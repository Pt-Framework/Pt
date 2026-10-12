---
proposal: http-oauth.md
---

# HTTP OAuth Server

Stage 6. May wait. Ships in `Pt-Http` when it ships. The stop
condition for the first slice is in-memory stores, `/authorize`,
`/token`, `/revoke`, and discovery. First-release tokens are opaque
values in `TokenStore`. JWT signing and `JwtAuthorizer` follow when
Ssl has HMAC and asymmetric verify. OpenID Connect discovery and
UserInfo wait for a separate concept.

No new library for stages 1 to 5. Types live in `Pt-Http`. Hash
lives in `Pt-Ssl` because HTTP already depends on TLS for HTTPS. A
`Pt-OAuth` library starts only if OpenID Connect and JWT issuance
become large enough to justify one.

When the API ships, the module page is `doc/pages/pt-http.md`.

## Baseline

There is no authorization server. Login, token issue, revocation,
and metadata are not HTTP services. There is no client store, no
authorization-code store, and no token store.

## Authorization server {#authorization-server}

An authorization server is four services, not a new server type.
Authorization-server endpoints are `Service` and `Responder` types.
They are not methods on `Authorizer`. Login at `/authorize` may
reuse an `Authorizer` on that servlet (Basic today, a form later).

```
GET  /authorize  -> AuthorizeService
POST /token      -> TokenService
POST /revoke     -> RevokeService
GET  /.well-known/oauth-authorization-server -> DiscoveryService
```

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

Production opaque tokens on a resource server use the same
`TokenStore` interface that `TokenListAuthorizer` uses for its
in-memory map.

Discovery returns the RFC 8414 document so an `OAuth2Client` can fill
`authorizationUri` and `tokenUri` from one issuer URL. That is
configuration sugar, not a new grant.

```
include/Pt/Http/OAuth2Server.h            Stage 6
```

## JWT {#jwt}

A JWT is three Base64url parts, header.payload.signature. The
resource server checks the signature and the claims (`iss`, `aud`,
`exp`, `nbf`, `sub`, `scope`) without a network round trip.

`JwtAuthorizer` verifies locally and stays synchronous. It ships
after Ssl can do HMAC-SHA256, RSA, and ECDSA, and after opaque
tokens already prove the header path. Issuance uses the same
`TokenStore` interface. The first Bearer check does not parse JWT.

OpenID Connect, including `id_token`, OIDC discovery, and UserInfo,
is a later layer. It is not required to ship Bearer and the common
grants.
