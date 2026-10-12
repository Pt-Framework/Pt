---
proposal: http-oauth.md
---

# HTTP OAuth Bearer

Stage 2 ships `BearerAuthentication` and `TokenListAuthorizer` in
`Pt-Http`. Stage 5 ships `IntrospectionAuthorizer` in `Pt-Http`.
`JwtAuthorizer` follows with JWT issuance.

Stop condition for stage 2: an `Http::Client` with `preAuthenticate`
calls an `Http::Server` whose servlet uses the list authorizer. A
good token reaches the responder context. A bad token returns 401
with a Bearer challenge.

Stop condition for stage 5: a resource server grants a request only
after a local introspection service returns `active: true`.
`IntrospectionAuthorizer` completes through
`Authorization::finished()`.

## Baseline

`Authentication::authenticate()` is synchronous and reactive. It
reads `WWW-Authenticate` on a `401` and writes `Authorization`.
Basic already has `preAuthenticate`. There is no shared entry that
attaches every registered scheme before the first send.

An asynchronous `beginAuthenticate` / `endAuthenticate` on the
client `Authenticator` does not exist. The asynchronous
`Authorization` object exists only on the server.

`Authorizer` returns `granted`. It does not place an accepted
identity on the request. There is no Bearer scheme and no in-memory
token list.

## Bearer attach {#bearer-attach}

`BearerAuthentication` is an `Authentication` named `"bearer"`. It
is registered on the existing `Authenticator` the same way Basic is.

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

`preAuthenticate` writes `Authorization: Bearer ...` before the
first send. The client does not wait for `401` the way Basic often
does. `authenticate` handles a `401` Bearer challenge when a token
is still present and usable. If the token is expired it returns
false. It does not refresh.

A `401` with

    WWW-Authenticate: Bearer error="invalid_token"

means the token is missing, expired, or revoked. Refresh or a new
grant is a second HTTP exchange against the authorization server.
It is not a header transform. That is why token acquisition does not
live inside `Authentication::authenticate()`. Grants belong on
`OAuth2Client`.

`Authenticator` gains `apply(Request&)` so a caller can attach every
registered scheme that supports preemptive credentials. The new
method is the shared entry. The 401 path stays for servers that
still challenge first.

An asynchronous `beginAuthenticate` / `endAuthenticate` on the
client `Authenticator` is reserved if a later scheme needs I/O.
Bearer does not use it.

```
include/Pt/Http/BearerAuthentication.h    or next to Authenticator.h
include/Pt/Http/Authenticator.h           apply()
```

## Bearer check {#bearer-check}

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

`onBeginAuthorize` parses `Authorization`. A Bearer token is handed
to `onAuthorizeToken`. A missing or foreign scheme writes `401` and

    WWW-Authenticate: Bearer realm="...", error="invalid_token"

`onAuthorizeToken` may finish synchronously (`granted` set, null
returned) or return an `Authorization` and complete on `finished()`,
the same pattern `Authorizer` already has for I/O.

`TokenListAuthorizer` is the Bearer analogue of
`BasicUserListAuthorizer`. It looks the token value up in an
in-memory map and fills `AuthContext`. Tests and small services use
it. Production opaque tokens use the same interface over
`TokenStore`.

Several servlets may share one authorizer. Scope differences belong
in derived authorizers or in the responders.

After a successful check the authorizer calls
`Request::setAuthContext`. That is the HTTP-side source for a
remoting `Principal` once the invocation-context work lands.

Opaque tokens and a list authorizer are enough to prove the header
path. JWT is not parsed in this stage.

```
include/Pt/Http/BearerAuthorizer.h
```

## Introspection {#introspection}

`IntrospectionAuthorizer` POSTs the token to an introspection
endpoint (RFC 7662) with client credentials and trusts `active`,
`sub`, `scope`, and `client_id`. The inner `Authorization` object
owns an `Http::Client`. `endAuthorize` reads `active` and the
claims. This is the first production path that needs the existing
asynchronous authorization object.

A later `JwtAuthorizer` verifies a signature and the registered
claims (`iss`, `aud`, `exp`, `nbf`, `sub`, `scope`) locally and
stays synchronous. It does not ship with the list authorizer.
