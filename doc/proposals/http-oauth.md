# HTTP OAuth

## Documents

- [Primitives](http-oauth-primitives.md)
- [Token](http-oauth-token.md)
- [Bearer](http-oauth-bearer.md)
- [Client](http-oauth-client.md)
- [Redirect](http-oauth-redirect.md)
- [Server](http-oauth-server.md)
- [Concept](../concepts/http-oauth.md)

## Open Tasks

- [query-params](http-oauth-primitives.md#query-params).
  Decoded, multi-value query fields.

- [base64url](http-oauth-primitives.md#base64url).
  Base64url without padding, beside the existing codec.

- [sha256](http-oauth-primitives.md#sha256).
  Public SHA-256 in Ssl, and constant-time comparison of verifiers
  and secrets.

- [access-token](http-oauth-token.md#access-token).
  An access token distinct from Credential: type, value, refresh
  value, scope, expiry.

- [auth-context](http-oauth-token.md#auth-context).
  The authorizer writes subject, client id, and scopes on the
  request. The responder reads them.

- [bearer-attach](http-oauth-bearer.md#bearer-attach).
  Bearer writes Authorization before the first send. apply()
  attaches every preemptive scheme. A 401 with an expired token
  does not refresh.

- [bearer-check](http-oauth-bearer.md#bearer-check).
  A known token reaches the responder context. An unknown token is
  401 with a Bearer challenge.

- [client-credentials](http-oauth-client.md#client-credentials).
  The grant client obtains and refreshes a token against a token
  endpoint. A failed grant keeps the previous token.

- [authorization-code](http-oauth-client.md#authorization-code).
  Authorization URL, PKCE, state check, and code exchange. A state
  mismatch fails the grant.

- [device-code](http-oauth-client.md#device-code).
  User code, verification URI, and poll until tokens arrive or the
  request expires.

- [redirect](http-oauth-redirect.md#redirect).
  Callback servlet and loopback receiver collect code and state.
  The grant client does not own the listener. Loopback binds
  127.0.0.1 and stops after the first match or a timeout.

- [introspection](http-oauth-bearer.md#introspection).
  The authorizer finishes only after introspection returns active
  true.

- [authorization-server](http-oauth-server.md#authorization-server).
  Authorize, token, revoke, and discovery as ordinary services over
  in-memory stores. First tokens are opaque.

- [jwt](http-oauth-server.md#jwt).
  JWT issuance and a local signature check behind the same store and
  authorizer split.

## Priority

Primitives first. They ship in the modules they touch: query fields
in Http, Base64url beside the existing codec, SHA-256 in Ssl. Token
and request context next, then Bearer on both sides with an
in-memory list. Those ship in Pt-Http.

Client credentials and refresh next. Authorization code, PKCE, and
the redirect receiver after that. Device authorization uses the same
client and does not need a receiver. Introspection after the
synchronous Bearer check. Those ship in Pt-Http.

Authorization server, opaque tokens, and discovery after a client
can already call a foreign token endpoint. JWT after opaque tokens
and after Ssl can sign and verify. Those may wait. A Pt-OAuth
library starts only if OpenID Connect and JWT issuance become large
enough to justify one.
