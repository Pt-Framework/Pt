# HTTP OAuth {#http-oauth}

This document is the design for OAuth 2 support in Platinum HTTP. It
is a framework concept, not an application sketch. OAuth is not a
third challenge scheme next to Basic. It is a token protocol: an
application obtains an access token from an authorization server, a
client attaches that token to requests, and a resource server accepts
or rejects the token. Those three jobs map onto three Pt types. A
fourth type, an authorization server built from ordinary HTTP
services, is optional later work.

The work is staged. Stage 0 adds primitives that Basic and OAuth both
need. Stages 1 to 3 make Platinum a resource-server and a token
client. Stages 4 to 6 add introspection, an authorization server, and
JWT. OpenID Connect stays out of the first release.

This chapter covers:

- [Purpose](#oauth-purpose)
- [How OAuth works](#oauth-how)
- [Roles](#oauth-roles)
- [Tokens](#oauth-tokens)
- [Grant types](#oauth-grants)
- [Authorization code in detail](#oauth-code)
- [PKCE](#oauth-pkce)
- [Using a token](#oauth-use)
- [Validating a token](#oauth-validate)
- [Why Basic is not enough](#oauth-gap)
- [Principles](#oauth-principles)
- [Object model](#oauth-model)
- [Access token and request context](#oauth-context)
- [Bearer authentication](#oauth-bearer-client)
- [OAuth2 client](#oauth-client)
- [Redirect receiver](#oauth-redirect)
- [Bearer authorizer](#oauth-bearer-server)
- [Authorization server](#oauth-as)
- [Supporting APIs](#oauth-support)
- [Stages](#oauth-stages)
- [Module layout](#oauth-layout)
- [Out of scope](#oauth-scope)

## Purpose {#oauth-purpose}

HTTP Basic sends a user name and a password on every request. OAuth 2
separates the party that owns the data from the party that calls the
API. The owner authorizes a client at an authorization server. The
client then calls the resource server with a short-lived access token.
The resource server never sees the owner's password.

Platinum already has the transport pieces: `Http::Client`,
`Http::Server`, `Servlet`, `Service`, `Responder`, `Authenticator`,
`Authorizer`, JSON, Base64, and TLS. It does not have a token type, a
grant client, a Bearer scheme, or a place on `Request` to store who
was accepted. Those are the gaps this design fills.

The first useful result is not a complete authorization server. It is
an access token that a client can attach and a resource server can
check, plus an asynchronous client that can obtain and refresh that
token. A Platinum process can then call GitHub, a company IdP, or a
later Platinum authorization server with the same types.

## How OAuth works {#oauth-how}

OAuth 1.0 signed every request with a shared secret. OAuth 2.0, the
protocol this design follows, does not. The client holds an access
token and sends it as an HTTP credential, almost always

    Authorization: Bearer <access_token>

How the client got the token is a separate protocol, the grant. How
the resource server decides that the token is valid is a third
protocol: a local lookup, an introspection request, or a JWT
signature check. Mixing those three protocols into one class is what
makes OAuth APIs hard to keep small.

The normative documents are RFC 6749 (framework), RFC 6750 (Bearer),
RFC 7636 (PKCE), RFC 7662 (introspection), RFC 7009 (revocation),
RFC 8252 (native apps), and RFC 8414 (authorization-server metadata).
OAuth 2.1, still a draft, drops the implicit grant and the resource-
owner password grant and requires PKCE for authorization code. This
design follows that subset.

OAuth is authorization, not authentication. A valid token means the
client may call a resource within a scope. It does not by itself prove
who the user is to the client application. OpenID Connect adds that
proof with an `id_token`. Platinum can grow an OIDC layer later. It is
not required to ship Bearer and the common grants.

## Roles {#oauth-roles}

OAuth names four roles. They are roles in a request, not process
boundaries. One Platinum binary can play more than one.

- Resource owner: the user or system that owns the data.
- Client: the application that wants access. In Pt this is
  `OAuth2Client` plus `Http::Client`.
- Authorization server (AS): issues tokens after the owner consents.
  In Pt this is a set of HTTP services, not an `Authorizer`.
- Resource server (RS): hosts the API. In Pt this is an `Http::Server`
  whose servlets carry a `BearerAuthorizer`.

A confidential client can keep a `client_secret`. A public client
(native app, desktop, many CLIs) cannot. Public clients use PKCE and
never treat a shipped secret as authentication.

## Tokens {#oauth-tokens}

An access token is an opaque string to everyone except the issuer and
the resource server. The client treats it as a password it must not
log. It has a type (`Bearer`), a lifetime, an optional refresh token,
and an optional scope string.

Two token shapes exist:

- Opaque: a random value. The resource server looks it up in a store
  or asks the authorization server (introspection).
- JWT (JSON Web Token): three Base64url parts, header.payload.signature.
  The resource server checks the signature and the claims (`iss`,
  `aud`, `exp`, `nbf`, `sub`, `scope`) without a network round trip.

A refresh token is not sent to the resource server. It is sent only to
the token endpoint to mint a new access token. Access tokens are
short-lived so a leak expires. Refresh tokens are longer-lived and
must be stored like a secret.

An authorization code is not a token for APIs. It is a one-time
credential that the client swaps for tokens at the token endpoint. It
travels through the user agent, so it is visible to the browser. That
is why it is short-lived and why PKCE binds it to the client that
started the flow.

## Grant types {#oauth-grants}

A grant is the way a client proves it may receive tokens.

### Authorization code

The interactive grant for applications that can open a browser. The
user authenticates at the authorization server and approves scopes.
The server redirects the user agent to the client's `redirect_uri`
with `code` and `state`. The client posts `code` to the token endpoint
and receives tokens. This is the default user-facing grant. PKCE is
required for public clients and recommended for confidential ones.

### Client credentials

The machine-to-machine grant. The client authenticates as itself with
`client_id` and `client_secret`. There is no resource owner in the
browser and no redirect. The token represents the client, not a user.

### Refresh token

Not a first grant from the owner's point of view. The client posts
`grant_type=refresh_token` and the refresh token to the token
endpoint. The authorization server returns a new access token and may
rotate the refresh token.

### Device authorization

For clients that have no browser, or no usable input: a CLI, a TV, a
service account helper. The client receives a user code and a
verification URI. The user authorizes on another device. The client
polls the token endpoint until the tokens arrive or the request
expires.

### Grants this design does not implement

The implicit grant returned a token in the URL fragment. It is
obsolete. The resource-owner password grant posted the user's password
to the client. OAuth 2.1 removes both. Platinum does not add them.

## Authorization code in detail {#oauth-code}

This is the grant that needs the most Pt machinery, because part of it
runs in a browser that Platinum does not control.

1. The client creates a random `state` (CSRF protection) and, with
   PKCE, a `code_verifier` and `code_challenge`.
2. The client sends the user to the authorization endpoint:

       GET {authorization_uri}
           ?response_type=code
           &client_id=...
           &redirect_uri=...
           &scope=...
           &state=...
           &code_challenge=...
           &code_challenge_method=S256

3. The user logs in and consents at the authorization server.
4. The authorization server responds to the browser with

       302 Location: {redirect_uri}?code=...&state=...

5. The user agent GETs that Location. Whoever owns `redirect_uri`
   reads `code` and `state`.
6. The client checks `state` against the value from step 1.
7. The client POSTs to the token endpoint:

       grant_type=authorization_code
       code=...
       redirect_uri=...
       client_id=...
       code_verifier=...

   A confidential client also sends HTTP Basic with `client_id` and
   `client_secret`, using the existing Basic client helper.
8. The authorization server returns JSON:

       {
         "access_token": "...",
         "token_type": "Bearer",
         "expires_in": 3600,
         "refresh_token": "...",
         "scope": "..."
       }

The important operational fact: the authorization server never opens a
connection to `Http::Client`. It only redirects the browser. The code
arrives as a normal HTTP request to `redirect_uri`. If that URI is a
public path on an already running `Http::Server`, a servlet reads the
query. If the application is a desktop, a CLI, or a service with no
public site, there is no such path until the process listens on one.
That is the loopback receiver described later.

## PKCE {#oauth-pkce}

PKCE (Proof Key for Code Exchange, RFC 7636) stops an attacker who can
see the redirect from exchanging the stolen code.

The client creates a high-entropy `code_verifier`. It sends

    code_challenge = BASE64URL(SHA256(code_verifier))

with the authorization request and keeps the verifier. The token
request includes the verifier. The authorization server hashes it and
compares it to the stored challenge. An attacker who intercepted the
code still lacks the verifier.

`Base64Codec` in Pt is RFC 4648 with padding and the `+/` alphabet.
PKCE needs Base64url: alphabet `-_`, no padding. SHA-256 is not on the
public Ssl API. Stage 0 adds both. Comparisons of verifiers and client
secrets are constant-time.

## Using a token {#oauth-use}

Once the client has an access token, every resource request carries

    Authorization: Bearer <access_token>

The client attaches the header before the first send. It does not wait
for `401` the way Basic often does. A `401` with

    WWW-Authenticate: Bearer error="invalid_token"

means the token is missing, expired, or revoked. The client refreshes
or starts a new grant, then sends the same resource request again.

Refresh is a second HTTP exchange against the authorization server. It
is not a header transform. That is why token acquisition does not live
inside `Authentication::authenticate()`.

## Validating a token {#oauth-validate}

The resource server reads `Authorization`, checks the scheme is
`Bearer`, and then:

- looks the opaque token up in a local store, or
- POSTs it to the introspection endpoint (RFC 7662) and trusts
  `active`, `sub`, `scope`, `client_id`, or
- verifies a JWT signature and the registered claims.

On failure it answers `401` and a `WWW-Authenticate` Bearer challenge.
On success it knows a subject, a client id, and scopes. Those facts
must travel with the request to the responder. A boolean `granted` is
not enough.

Scope checks can sit in the authorizer (this servlet requires
`write`) or in the responder (`request.authContext()->hasScope(...)`).
The authorizer is the coarse door. The responder is the fine door.

## Why Basic is not enough {#oauth-gap}

`Authenticator` and `Authorizer` are the right split: the client
writes headers after a challenge, the server checks a servlet. Basic
fits that split. Three properties of the current API do not.

`Credential` is a user name and a password. An access token has a
type, a value, an expiry, a refresh value, and a scope. Extending
`Credential` with those fields would make Basic carry dead weight.
A second value type is cheaper.

`Authentication::authenticate()` is synchronous and reactive. It reads
`WWW-Authenticate` on a `401` and writes `Authorization`. OAuth
attaches a Bearer header before the first request and obtains tokens
with additional HTTP. Putting grant I/O inside `authenticate()` would
block the caller or force every authentication plugin onto the
asynchronous `Authorization` object that today exists only on the
server.

`Authorizer` returns `granted`. The server is multithreaded. The
accepted identity cannot live on the authorizer. It has to live on the
`Request` that the responder already receives.

The remoting invocation-context concept depends on that identity
crossing the HTTP boundary as a principal. OAuth is how HTTP will
often produce it. Remoting still does not parse tokens.

## Principles {#oauth-principles}

Obtain, attach, and check stay three types. `OAuth2Client` talks to an
authorization server. `BearerAuthentication` writes a header.
`BearerAuthorizer` reads a header. None of the three is a facade for
the other two.

`Http::Client` stays one request and one reply. It does not grow a
token cache, a grant state machine, or a browser helper.

`Credential` stays Basic. `AccessToken` is the OAuth value.

Grants that need a user agent use a `RedirectReceiver`. The grant
client does not own a listening server. A deployed callback servlet
and a loopback listener implement the same receiver interface.

Authorization-server endpoints are `Service` and `Responder` types.
They are not methods on `Authorizer`. Login at `/authorize` may reuse
an `Authorizer` on that servlet (Basic today, a form later).

No provider SDK. Google, GitHub, and a company IdP differ by URI and
by a few token-endpoint quirks. The core types take those URIs as
configuration. Preset config helpers may appear later. They are not
the API.

Secrets are not logged. Token comparison is constant-time. HTTPS is
required for real deployments; loopback HTTP is allowed only for the
native redirect specified by RFC 8252.

## Object model {#oauth-model}

```
Resource owner (browser or other device)
        |
        | authorize / consent
        v
Authorization server          Token endpoint JSON
  /authorize  /token                 |
  /revoke     /.well-known           |
        ^                            v
        |                     OAuth2Client ---- AccessToken
        |                            |
        |                     BearerAuthentication
        |                            |
        |                            v
        |                     Http::Client  -->  resource request
        |
        v
Http::Server + Servlet + BearerAuthorizer
        |
        | AuthContext on Request
        v
Responder / remoting Invocation principal
```

## Access token and request context {#oauth-context}

```cpp
class AccessToken
{
    public:
        const std::string& type() const;       // "Bearer"
        const std::string& value() const;
        const std::string& refreshValue() const;
        const std::string& scope() const;
        const DateTime& expires() const;
        bool isExpired(const Timespan& skew = Timespan(30, 0)) const;
};

class AuthContext
{
    public:
        const std::string& subject() const;
        const std::string& clientId() const;
        bool hasScope(const std::string& scope) const;
        const AccessToken& token() const;
};
```

`Request` stores the context for the current exchange:

```cpp
void Request::setAuthContext(const AuthContext& ctx);
const AuthContext* Request::authContext() const; // 0 if anonymous
```

The authorizer writes it. The responder reads it. After a `101`
upgrade the HTTP authorizer has already run; WebSocket code that needs
identity reads the context from the upgrade request or performs its
own check on the first frame. The design does not invent a second
Bearer scheme for WebSocket.

## Bearer authentication {#oauth-bearer-client}

`BearerAuthentication` is an `Authentication` named `"bearer"`. It is
registered on the existing `Authenticator` the same way Basic is.

```cpp
class BearerAuthentication : public Authentication
{
    public:
        BearerAuthentication();

        void setToken(const AccessToken& token);
        void preAuthenticate(Request& request);

        virtual bool authenticate(const Credentials& credentials,
                                  Request& request,
                                  const Reply& reply);
};
```

`preAuthenticate` writes `Authorization: Bearer ...` before the first
send. `authenticate` handles a `401` Bearer challenge when a token is
still present and usable. If the token is expired it returns false.
It does not refresh.

`Authenticator` gains `apply(Request&)` so a caller can attach every
registered scheme that supports preemptive credentials. Basic already
has `preAuthenticate`. The new method is the shared entry. The 401
path stays for servers that still challenge first.

An asynchronous `beginAuthenticate` / `endAuthenticate` on the client
`Authenticator` is reserved if a later scheme needs I/O. Bearer does
not use it. Grants belong on `OAuth2Client`.

## OAuth2 client {#oauth-client}

`OAuth2Client` is a `Connectable` bound to an `EventLoop`, like
`Http::Client`. It owns or borrows an `Http::Client` aimed at the
token endpoint. Configuration is data:

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
response is JSON parsed with `Pt::Json`. Confidential clients set
`Authorization: Basic` on that request through
`BasicAuthentication::preAuthenticate`.

`apply` copies the current access token onto a resource `Request`
through `BearerAuthentication`. Application code that already holds an
`Authenticator` can set the token on the Bearer plugin instead.

Failed grants emit `failed()` and leave the previous token in place.
Callers decide whether to retry.

## Redirect receiver {#oauth-redirect}

`authorizationUrl()` only builds a URI. Something must still collect
`code` and `state` from the redirect.

```cpp
class RedirectReceiver
{
    public:
        virtual ~RedirectReceiver()
        {}

        virtual void beginListen() = 0;
        Signal<RedirectReceiver&>& received();
        const std::string& code() const;
        const std::string& state() const;
        const std::string& error() const;
};
```

Two implementations cover the two deployments.

`CallbackServlet` is a `MapUrl` on an already listening `Http::Server`.
The redirect URI is a public or intranet HTTPS path. This is the
server-side client case.

`LoopbackReceiver` starts a short-lived `Http::Server` on
`127.0.0.1` and a chosen or ephemeral port, maps the redirect path,
and stops listening after the first matching GET (or after a timeout).
The redirect URI looks like `http://127.0.0.1:7890/callback`. Binding
loopback rather than `0.0.0.0` keeps the callback off the LAN. This is
the native-app case in RFC 8252.

The loopback server exists only because the authorization server
answers the browser, not the Platinum client. Without a listener at
`redirect_uri` the `302` completes in the browser and the process
never sees the code. Client-credentials and device-code grants do not
use a receiver. Custom URI schemes (`myapp://callback`) are a third
receiver, platform-specific, and out of the first implementation.

## Bearer authorizer {#oauth-bearer-server}

```cpp
class BearerAuthorizer : public Authorizer
{
    protected:
        virtual Authorization* onBeginAuthorize(const Request& req,
                                                Reply& reply,
                                                bool& granted);

        virtual Authorization* onAuthorizeToken(const AccessToken& token,
                                                AuthContext& ctx,
                                                bool& granted) = 0;
};
```

`onBeginAuthorize` parses `Authorization`. A Bearer token is handed to
`onAuthorizeToken`. A missing or foreign scheme writes `401` and

    WWW-Authenticate: Bearer realm="...", error="invalid_token"

`onAuthorizeToken` may finish synchronously (`granted` set, null
returned) or return an `Authorization` and complete on `finished()`,
the same pattern `Authorizer` already has for I/O.

Two derived types ship with the framework:

`TokenListAuthorizer` is the Bearer analogue of
`BasicUserListAuthorizer`. It looks the token value up in an
in-memory map and fills `AuthContext`. Tests and small services use
it. Production opaque tokens use the same interface over `TokenStore`.

`IntrospectionAuthorizer` POSTs the token to an introspection endpoint
with client credentials. The inner `Authorization` object owns a
`Http::Client`. `endAuthorize` reads `active` and the claims. This is
the first production path that needs the existing asynchronous
authorization object.

A later `JwtAuthorizer` verifies locally and stays synchronous.

Several servlets may share one authorizer. Scope differences belong in
derived authorizers or in the responders.

After a successful check the authorizer calls
`Request::setAuthContext`. That is the HTTP-side source for a remoting
`Principal` once the invocation-context work lands.

## Authorization server {#oauth-as}

An authorization server is four services, not a new server type:

    GET  /authorize  -> AuthorizeService
    POST /token      -> TokenService
    POST /revoke     -> RevokeService
    GET  /.well-known/oauth-authorization-server -> DiscoveryService

Stores are interfaces with in-memory defaults, like the Basic user
list:

```cpp
class OAuth2ClientStore
{
    public:
        virtual bool find(const std::string& clientId,
                          OAuth2RegisteredClient& out) = 0;
};

class AuthorizationCodeStore; // code, client, redirect, verifier, subject, expiry
class TokenStore;             // access, refresh, revoke, lookup
```

`AuthorizeResponder` validates `client_id`, `redirect_uri`,
`response_type=code`, `state`, and `code_challenge`. The resource
owner authenticates through the servlet authorizer. On success the
responder stores a code and redirects.

`TokenResponder` accepts `authorization_code` with PKCE,
`client_credentials`, and `refresh_token`. The reply is JSON with
`Cache-Control: no-store`. First-release tokens are opaque values in
`TokenStore`. JWT issuance is a later token format behind the same
store interface.

Discovery returns the RFC 8414 document so an `OAuth2Client` can fill
`authorizationUri` and `tokenUri` from one issuer URL. That is Stage 6
configuration sugar, not a new grant.

## Supporting APIs {#oauth-support}

`Request::qparams()` is a raw string. `/authorize` and the loopback
callback need decoded, multi-value query fields. Add `QueryParams` as
a small HTTP helper. It is useful outside OAuth.

`Base64UrlCodec` sits next to `Base64Codec`. PKCE and JWT both need
it.

SHA-256 (and later HMAC-SHA256, RSA, ECDSA for JWT) belong in Ssl, as
`Pt::Ssl::Hash` or a similarly small type that wraps the library
already used by `Ssl::Context`. `Pt-Http` must not include the TLS
library headers.

Clock skew for `exp` / `nbf` uses `Pt::DateTime` and `Pt::Timespan`.

## Stages {#oauth-stages}

Each stage has a stop condition. The next stage does not start until
that condition is met. Stage 0 ships in the modules it touches.
Stages 1 to 4 ship in `Pt-Http`. Stages 5 and 6 may wait.

| Stage | Ships in | Stop condition |
|---|---|---|
| 0 | Http, Ssl, Core | `QueryParams`, `Base64UrlCodec`, SHA-256 hash |
| 1 | Http | `AccessToken`, `AuthContext` on `Request` |
| 2 | Http | `BearerAuthentication`, `TokenListAuthorizer` |
| 3 | Http | `OAuth2Client` client-credentials and refresh |
| 4 | Http | Authorization code, PKCE, `RedirectReceiver` |
| 5 | Http | `IntrospectionAuthorizer` |
| 6 | Http | AS services and opaque `TokenStore`; JWT later |

### Stage 0 - Primitives

Add `QueryParams`, `Base64UrlCodec`, and a SHA-256 hash type. Tests
round-trip Base64url without padding and compute the PKCE S256
challenge for a known verifier. No OAuth types yet.

### Stage 1 - Token and context

Add `AccessToken` and `AuthContext`. `Request` stores an optional
context. Tests attach a context on a fake request and read it back.
`Credential` is unchanged.

### Stage 2 - Bearer on both sides

`BearerAuthentication` writes and retries with a configured token.
`BearerAuthorizer` plus `TokenListAuthorizer` accept or reject that
token and fill `AuthContext`. Stop condition: an `Http::Client` with
`preAuthenticate` calls an `Http::Server` whose servlet uses the list
authorizer; a good token reaches the responder context, a bad token
returns 401 with a Bearer challenge.

### Stage 3 - Non-interactive grants

`OAuth2Client` implements client credentials and refresh against a
test double of the token endpoint (a local `Http::Server` returning
fixed JSON). Stop condition: the client stores an `AccessToken` with
expiry and can refresh it on the loop.

### Stage 4 - Authorization code

PKCE generation, `authorizationUrl()`, `beginAuthorizationCode`, and
both receivers. Stop condition: a loopback receiver running in the
same process as a fake `/authorize` that redirects to it; the client
exchanges the captured code and ends with an access token. `state`
mismatch fails the grant.

### Stage 5 - Introspection

`IntrospectionAuthorizer` completes through `Authorization::finished()`.
Stop condition: a resource server grants a request only after a local
introspection service returns `active: true`.

### Stage 6 - Authorization server and JWT

In-memory stores, `/authorize`, `/token`, `/revoke`, discovery.
Opaque tokens first. JWT signing and `JwtAuthorizer` follow when Ssl
has HMAC and asymmetric verify. OIDC discovery and UserInfo wait for
a separate concept.

## Module layout {#oauth-layout}

```
include/Pt/Http/AccessToken.h
include/Pt/Http/AuthContext.h
include/Pt/Http/QueryParams.h
include/Pt/Http/BearerAuthentication.h    or next to Authenticator.h
include/Pt/Http/OAuth2Client.h
include/Pt/Http/RedirectReceiver.h
include/Pt/Http/BearerAuthorizer.h
include/Pt/Http/OAuth2Server.h            Stage 6
include/Pt/Http/Request.h                 AuthContext accessors
include/Pt/Http/Authenticator.h           apply()
include/Pt/Ssl/                           Hash / SHA-256
include/Pt/Base64Codec.h                  sibling Base64UrlCodec
src/Pt-Http/
src/Pt-Http/tests/
doc/concepts/http-oauth.md
doc/pages/pt-http.md                      module page when the API ships
```

No new library for stages 1 to 5. Types live in `Pt-Http`. Hash lives
in `Pt-Ssl` because HTTP already depends on TLS for HTTPS. A
`Pt-OAuth` library starts only if OIDC and JWT issuance become large
enough to justify one.

## Out of scope {#oauth-scope}

This design does not add OpenID Connect, SAML, or WebAuthn.

It does not put grant state on `Http::Client`.

It does not implement implicit or password grants.

It does not ship provider-specific clients as core types.

It does not parse JWT in Stage 2. Opaque tokens and a list authorizer
are enough to prove the header path.

It does not replace remoting filters or `Remoting::Principal`. HTTP
produces an `AuthContext`. Remoting copies a principal from it.

It does not add cookie sessions. A session authorizer can be a later
plugin that writes the same `AuthContext`.

It does not listen on `0.0.0.0` for native redirects and it does not
require a custom URI scheme in the first receiver implementation.
