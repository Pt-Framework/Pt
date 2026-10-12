---
proposal: http-oauth.md
---

# HTTP OAuth Client

Stage 3 ships client credentials and refresh in `Pt-Http`. Stage 4
ships authorization code and PKCE in `Pt-Http`. Device authorization
uses the same client and does not need a receiver.

Stop condition for stage 3: `OAuth2Client` implements client
credentials and refresh against a test double of the token endpoint
(a local `Http::Server` returning fixed JSON). The client stores an
`AccessToken` with expiry and can refresh it on the loop.

Stop condition for stage 4: PKCE generation, `authorizationUrl()`,
and `beginAuthorizationCode`. A loopback receiver running in the
same process as a fake `/authorize` that redirects to it lets the
client exchange the captured code and end with an access token. A
`state` mismatch fails the grant.

## Baseline

`Http::Client` is one request and one reply. It has no token cache,
no grant state machine, and no browser helper. There is no grant
client.

`Authentication::authenticate()` must not grow grant I/O. Putting
that I/O inside `authenticate()` would block the caller or force
every authentication plugin onto the asynchronous `Authorization`
object that today exists only on the server.

## Client credentials {#client-credentials}

`OAuth2Client` is a `Connectable` bound to an `EventLoop`, like
`Http::Client`. It owns or borrows an `Http::Client` aimed at the
token endpoint. Configuration is data. No provider SDK. Google,
GitHub, and a company identity provider differ by URI and by a few
token-endpoint quirks. The core types take those URIs as
configuration. Preset config helpers may appear later. They are not
the API.

```cpp
class OAuth2Client : public Connectable, private NonCopyable
{
    public:
        struct Config
        {
            std::string clientId;
            std::string clientSecret;      // empty => public client + PKCE
            std::string authorizationUri;
            std::string tokenUri;
            std::string redirectUri;
            std::string scope;
        };

        explicit OAuth2Client(System::EventLoop& loop);
        void setSecure(Ssl::Context& ctx);
        void setConfig(const Config& cfg);

        std::string authorizationUrl();    // state + PKCE verifier

        void beginAuthorizationCode(const std::string& code);
        AccessToken endAuthorizationCode();

        void beginClientCredentials();
        AccessToken endClientCredentials();

        void beginRefresh();
        AccessToken endRefresh();

        void beginDeviceAuthorization();
        DeviceCode endDeviceAuthorization();
        void beginDevicePoll();
        AccessToken endDevicePoll();

        Signal<OAuth2Client&>& tokenReceived();
        Signal<OAuth2Client&>& failed();

        const AccessToken& token() const;
        void apply(Request& request);
};
```

The token request body is `application/x-www-form-urlencoded`. The
response is JSON parsed with `Pt::Json`. A confidential client
authenticates as itself with `client_id` and `client_secret` and
sets `Authorization: Basic` on that request through
`BasicAuthentication::preAuthenticate`. There is no resource owner
in the browser and no redirect. The token represents the client, not
a user. An empty `clientSecret` means a public client, which uses
PKCE and never treats a shipped secret as authentication.

Refresh posts `grant_type=refresh_token` and the refresh token to
the token endpoint. The authorization server returns a new access
token and may rotate the refresh token. Refresh is not a first grant
from the owner's point of view, and it is not a header transform.

Failed grants emit `failed()` and leave the previous token in place.
Callers decide whether to retry.

`apply` copies the current access token onto a resource `Request`
through `BearerAuthentication`. Application code that already holds
an `Authenticator` can set the token on the Bearer plugin instead.

Secrets are not logged. Token comparison is constant-time. HTTPS is
required for real deployments.

```
include/Pt/Http/OAuth2Client.h
src/Pt-Http/
src/Pt-Http/tests/
```

## Authorization code {#authorization-code}

The interactive grant for applications that can open a browser. The
user authenticates at the authorization server and approves scopes.
The server redirects the user agent to the client's `redirect_uri`
with `code` and `state`. The client posts `code` to the token
endpoint and receives tokens. This is the default user-facing grant.
PKCE is required for public clients and recommended for confidential
ones.

`authorizationUrl()` builds the authorization URI. It creates a
random `state` (CSRF protection) and, with PKCE, a `code_verifier`
and `code_challenge`. It does not collect the redirect. Something
else must still collect `code` and `state`.

```
GET {authorization_uri}
    ?response_type=code
    &client_id=...
    &redirect_uri=...
    &scope=...
    &state=...
    &code_challenge=...
    &code_challenge_method=S256
```

The challenge is `BASE64URL(SHA256(code_verifier))`. The client
keeps the verifier. The token request includes it. The authorization
server hashes it and compares it to the stored challenge. An
attacker who intercepted the code still lacks the verifier.
Comparisons of verifiers and client secrets are constant-time.

The client checks `state` against the value it created. A mismatch
fails the grant.

The token request is:

```
grant_type=authorization_code
code=...
redirect_uri=...
client_id=...
code_verifier=...
```

A confidential client also sends HTTP Basic with `client_id` and
`client_secret`, using the existing Basic client helper.

The token response is JSON:

```
{
  "access_token": "...",
  "token_type": "Bearer",
  "expires_in": 3600,
  "refresh_token": "...",
  "scope": "..."
}
```

An authorization code is not a token for APIs. It is a one-time
credential that the client swaps for tokens at the token endpoint.
It travels through the user agent, so it is visible to the browser.
That is why it is short-lived and why PKCE binds it to the client
that started the flow.

The authorization server never opens a connection to `Http::Client`.
It only redirects the browser. The code arrives as a normal HTTP
request to `redirect_uri`.

The implicit grant and the resource-owner password grant are not
implemented. OAuth 2.1 removes both.

## Device authorization {#device-code}

For clients that have no browser, or no usable input: a command-line
tool, a TV, a service account helper. The client receives a user
code and a verification URI. The user authorizes on another device.
The client polls the token endpoint until the tokens arrive or the
request expires.

`beginDeviceAuthorization` / `endDeviceAuthorization` return the
`DeviceCode`. `beginDevicePoll` / `endDevicePoll` wait for the
access token. This grant does not use a redirect receiver.
