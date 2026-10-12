# HTTP OAuth {#http-oauth}

OAuth 2 support in Platinum HTTP is a framework concept, not an
application sketch. OAuth is not a third challenge scheme next to
Basic. It is a token protocol: an application obtains an access token
from an authorization server, a client attaches that token to
requests, and a resource server accepts or rejects the token. Those
three jobs stay three separate capabilities. An authorization server
built from ordinary HTTP services is optional later work.

OpenID Connect stays out of the first release. The first useful
result is not a complete authorization server. It is an access token
that a client can attach and a resource server can check, plus a
client that can obtain and refresh that token. A Platinum process can
then call GitHub, a company identity provider, or a later Platinum
authorization server with the same model.

## Purpose {#oauth-purpose}

HTTP Basic sends a user name and a password on every request. OAuth 2
separates the party that owns the data from the party that calls the
API. The owner authorizes a client at an authorization server. The
client then calls the resource server with a short-lived access token.
The resource server never sees the owner's password.

Platinum already has the transport pieces: an HTTP client and server,
servlets, services, responders, authenticators, authorizers, JSON,
Base64, and TLS. It does not have a token type, a grant client, a
Bearer scheme, or a place on the request to store who was accepted.
Those are the gaps this model fills.

## How OAuth works {#oauth-how}

OAuth 1.0 signed every request with a shared secret. OAuth 2.0, the
protocol this model follows, does not. The client holds an access
token and sends it as an HTTP credential, almost always

    Authorization: Bearer <access_token>

How the client got the token is a separate protocol, the grant. How
the resource server decides that the token is valid is a third
protocol: a local lookup, an introspection request, or a JWT
signature check. Mixing those three protocols into one type is what
makes OAuth APIs hard to keep small.

The normative documents are RFC 6749 (framework), RFC 6750 (Bearer),
RFC 7636 (PKCE), RFC 7662 (introspection), RFC 7009 (revocation),
RFC 8252 (native apps), and RFC 8414 (authorization-server metadata).
OAuth 2.1, still a draft, drops the implicit grant and the resource-
owner password grant and requires PKCE for authorization code. This
model follows that subset.

OAuth is authorization, not authentication. A valid token means the
client may call a resource within a scope. It does not by itself prove
who the user is to the client application. OpenID Connect adds that
proof with an `id_token`. Platinum can grow an OpenID Connect layer
later. It is not required to ship Bearer and the common grants.

## Roles {#oauth-roles}

OAuth names four roles. They are roles in a request, not process
boundaries. One Platinum binary can play more than one.

- Resource owner: the user or system that owns the data.
- Client: the application that wants access.
- Authorization server: issues tokens after the owner consents. In
  Platinum this is a set of HTTP services, not an authorizer.
- Resource server: hosts the API. In Platinum this is an HTTP server
  whose servlets carry a Bearer check.

A confidential client can keep a client secret. A public client
(native app, desktop, many command-line tools) cannot. Public clients
use PKCE and never treat a shipped secret as authentication.

## Tokens {#oauth-tokens}

An access token is an opaque string to everyone except the issuer and
the resource server. The client treats it as a password it must not
log. It has a type (Bearer), a lifetime, an optional refresh token,
and an optional scope string.

Two token shapes exist:

- Opaque: a random value. The resource server looks it up in a store
  or asks the authorization server (introspection).
- JWT (JSON Web Token): three Base64url parts,
  header.payload.signature. The resource server checks the signature
  and the claims (`iss`, `aud`, `exp`, `nbf`, `sub`, `scope`) without
  a network round trip.

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
The server redirects the user agent to the client's redirect URI with
a code and a state. The client posts the code to the token endpoint
and receives tokens. This is the default user-facing grant. PKCE is
required for public clients and recommended for confidential ones.

### Client credentials

The machine-to-machine grant. The client authenticates as itself with
a client id and a client secret. There is no resource owner in the
browser and no redirect. The token represents the client, not a user.

### Refresh token

Not a first grant from the owner's point of view. The client posts
`grant_type=refresh_token` and the refresh token to the token
endpoint. The authorization server returns a new access token and may
rotate the refresh token.

### Device authorization

For clients that have no browser, or no usable input: a command-line
tool, a TV, a service account helper. The client receives a user code
and a verification URI. The user authorizes on another device. The
client polls the token endpoint until the tokens arrive or the request
expires.

### Grants this model does not implement

The implicit grant returned a token in the URL fragment. It is
obsolete. The resource-owner password grant posted the user's password
to the client. OAuth 2.1 removes both. Platinum does not add them.

## Authorization code in detail {#oauth-code}

This is the grant that needs the most machinery, because part of it
runs in a browser that Platinum does not control.

1. The client creates a random state (CSRF protection) and, with
   PKCE, a code verifier and a code challenge.
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

5. The user agent GETs that Location. Whoever owns the redirect URI
   reads the code and the state.
6. The client checks the state against the value from step 1.
7. The client POSTs to the token endpoint:

       grant_type=authorization_code
       code=...
       redirect_uri=...
       client_id=...
       code_verifier=...

   A confidential client also sends HTTP Basic with the client id and
   the client secret, using the existing Basic client helper.
8. The authorization server returns JSON:

       {
         "access_token": "...",
         "token_type": "Bearer",
         "expires_in": 3600,
         "refresh_token": "...",
         "scope": "..."
       }

The important operational fact: the authorization server never opens a
connection to the Platinum HTTP client. It only redirects the browser.
The code arrives as a normal HTTP request to the redirect URI. If that
URI is a public path on an already running HTTP server, a servlet
reads the query. If the application is a desktop, a command-line tool,
or a service with no public site, there is no such path until the
process listens on one. That is the loopback receiver.

## PKCE {#oauth-pkce}

PKCE (Proof Key for Code Exchange, RFC 7636) stops an attacker who can
see the redirect from exchanging the stolen code.

The client creates a high-entropy code verifier. It sends

    code_challenge = BASE64URL(SHA256(code_verifier))

with the authorization request and keeps the verifier. The token
request includes the verifier. The authorization server hashes it and
compares it to the stored challenge. An attacker who intercepted the
code still lacks the verifier.

The existing Base64 codec is RFC 4648 with padding and the `+/`
alphabet. PKCE needs Base64url: alphabet `-_`, no padding. SHA-256 is
not on the public Ssl API. Both are prerequisites. Comparisons of
verifiers and client secrets are constant-time.

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
inside the reactive authentication step that answers a challenge.

## Validating a token {#oauth-validate}

The resource server reads `Authorization`, checks the scheme is
Bearer, and then:

- looks the opaque token up in a local store, or
- POSTs it to the introspection endpoint (RFC 7662) and trusts
  `active`, `sub`, `scope`, `client_id`, or
- verifies a JWT signature and the registered claims.

On failure it answers `401` and a `WWW-Authenticate` Bearer challenge.
On success it knows a subject, a client id, and scopes. Those facts
must travel with the request to the responder. A boolean granted is
not enough.

Scope checks can sit in the authorizer (this servlet requires
`write`) or in the responder (the request context reports whether it
has a scope). The authorizer is the coarse door. The responder is the
fine door.

## Why Basic is not enough {#oauth-gap}

The authenticator and the authorizer are the right split: the client
writes headers after a challenge, the server checks a servlet. Basic
fits that split. Three properties of that split do not fit OAuth.

A credential is a user name and a password. An access token has a
type, a value, an expiry, a refresh value, and a scope. Extending the
credential with those fields would make Basic carry dead weight. A
second value type is cheaper.

The reactive authentication step is synchronous. It reads
`WWW-Authenticate` on a `401` and writes `Authorization`. OAuth
attaches a Bearer header before the first request and obtains tokens
with additional HTTP. Putting grant I/O inside that step would block
the caller or force every authentication plugin onto the asynchronous
authorization object that today exists only on the server.

An authorizer returns granted. The server is multithreaded. The
accepted identity cannot live on the authorizer. It has to live on the
request that the responder already receives.

The remoting invocation-context model depends on that identity
crossing the HTTP boundary as a principal. OAuth is how HTTP will
often produce it. Remoting still does not parse tokens.

## Principles {#oauth-principles}

Obtain, attach, and check stay three capabilities. The grant client
talks to an authorization server. Bearer authentication writes a
header. The Bearer authorizer reads a header. None of the three is a
facade for the other two.

The HTTP client stays one request and one reply. It does not grow a
token cache, a grant state machine, or a browser helper.

The credential stays Basic. The access token is the OAuth value.

Grants that need a user agent use a redirect receiver. The grant
client does not own a listening server. A deployed callback servlet
and a loopback listener implement the same receiver.

Authorization-server endpoints are service and responder types. They
are not methods on an authorizer. Login at the authorize path may
reuse an authorizer on that servlet (Basic today, a form later).

No provider SDK. Google, GitHub, and a company identity provider
differ by URI and by a few token-endpoint quirks. The core takes those
URIs as configuration. Preset config helpers may appear later. They
are not the API.

Secrets are not logged. Token comparison is constant-time. HTTPS is
required for real deployments. Loopback HTTP is allowed only for the
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
        |                       grant client ---- access token
        |                            |
        |                     Bearer authentication
        |                            |
        |                            v
        |                     HTTP client  -->  resource request
        |
        v
HTTP server + servlet + Bearer authorizer
        |
        | auth context on the request
        v
Responder / remoting invocation principal
```

## Out of scope {#oauth-scope}

This model does not add OpenID Connect, SAML, or WebAuthn.

It does not put grant state on the HTTP client.

It does not implement implicit or password grants.

It does not ship provider-specific clients as core types.

It does not parse JWT in the first Bearer check. Opaque tokens and a
list authorizer are enough to prove the header path.

It does not replace remoting filters or the remoting principal. HTTP
produces an auth context. Remoting copies a principal from it.

It does not add cookie sessions. A session authorizer can be a later
plugin that writes the same auth context.

It does not listen on all interfaces for native redirects and it does
not require a custom URI scheme in the first receiver.
