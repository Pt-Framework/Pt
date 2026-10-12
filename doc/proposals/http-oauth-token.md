---
proposal: http-oauth.md
---

# HTTP OAuth Token

Stage 1. Ships in `Pt-Http`. The stop condition is `AccessToken` and
`AuthContext` on `Request`. Tests attach a context on a fake request
and read it back. `Credential` is unchanged.

## Baseline

`Credential` is a user name and a password. An access token has a
type, a value, an expiry, a refresh value, and a scope. Extending
`Credential` with those fields would make Basic carry dead weight.

`Authorizer` returns `granted`. The server is multithreaded. The
accepted identity cannot live on the authorizer. It has to live on
the `Request` that the responder already receives.

The remoting invocation-context concept depends on that identity
crossing the HTTP boundary as a principal. OAuth is how HTTP will
often produce it. Remoting still does not parse tokens. HTTP
produces an `AuthContext`. Remoting copies a principal from it.

## Access token {#access-token}

`AccessToken` is the OAuth value. `Credential` stays Basic.

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
```

The client treats the value as a password it must not log. A refresh
value is not sent to the resource server. It is sent only to the
token endpoint. Access tokens are short-lived so a leak expires.
Refresh tokens are longer-lived and must be stored like a secret.

```
include/Pt/Http/AccessToken.h
```

## Auth context {#auth-context}

`Request` stores the context for the current exchange. The authorizer
writes it. The responder reads it.

```cpp
class AuthContext
{
    public:
        const std::string& subject() const;
        const std::string& clientId() const;
        bool hasScope(const std::string& scope) const;
        const AccessToken& token() const;
};

void Request::setAuthContext(const AuthContext& ctx);
const AuthContext* Request::authContext() const; // 0 if anonymous
```

A boolean `granted` is not enough. On success the server knows a
subject, a client id, and scopes. Those facts must travel with the
request to the responder.

Scope checks can sit in the authorizer (this servlet requires
`write`) or in the responder
(`request.authContext()->hasScope(...)`). The authorizer is the
coarse door. The responder is the fine door.

After a `101` upgrade the HTTP authorizer has already run. WebSocket
code that needs identity reads the context from the upgrade request
or performs its own check on the first frame. The design does not
invent a second Bearer scheme for WebSocket.

A cookie-session authorizer can be a later plugin that writes the
same `AuthContext`. Cookie sessions are out of scope here.

```
include/Pt/Http/AuthContext.h
include/Pt/Http/Request.h             AuthContext accessors
```
