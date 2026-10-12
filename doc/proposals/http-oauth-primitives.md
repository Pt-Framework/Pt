---
proposal: http-oauth.md
---

# HTTP OAuth Primitives

Stage 0. These types ship in the modules they touch. No OAuth types
yet. The stop condition is a query helper, a Base64url codec, and a
SHA-256 hash. Tests round-trip Base64url without padding and compute
the PKCE S256 challenge for a known verifier.

## Baseline

`Request::qparams()` is a raw string. `/authorize` and the loopback
callback need decoded, multi-value query fields.

`Base64Codec` is RFC 4648 with padding and the `+/` alphabet. PKCE
and JWT need Base64url: alphabet `-_`, no padding.

SHA-256 is not on the public Ssl API. `Pt-Http` must not include the
TLS library headers. HTTP already depends on TLS for HTTPS, so the
hash lives in `Pt-Ssl` rather than in a new library.

## Query parameters {#query-params}

Add `QueryParams` as a small HTTP helper. It decodes multi-value
query fields. It is useful outside OAuth.

```
include/Pt/Http/QueryParams.h
```

## Base64url {#base64url}

`Base64UrlCodec` sits next to `Base64Codec`.

```
include/Pt/Base64Codec.h              sibling Base64UrlCodec
```

## SHA-256 {#sha256}

SHA-256, and later HMAC-SHA256, RSA, and ECDSA for JWT, belong in
Ssl, as `Pt::Ssl::Hash` or a similarly small type that wraps the
library already used by `Ssl::Context`.

Comparisons of verifiers and client secrets are constant-time.
Clock skew for `exp` and `nbf` uses `Pt::DateTime` and
`Pt::Timespan`. Those clock types already exist.

```
include/Pt/Ssl/                      Hash / SHA-256
```
