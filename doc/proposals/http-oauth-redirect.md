---
proposal: http-oauth.md
---

# HTTP OAuth Redirect

Stage 4, with authorization code. Ships in `Pt-Http`. The stop
condition is both receivers: a loopback receiver running in the same
process as a fake `/authorize` that redirects to it, and a callback
servlet on an already listening server. The client exchanges the
captured code and ends with an access token.

## Baseline

There is no listener that collects `code` and `state` from a
redirect. `Http::Client` does not see the authorization server. The
authorization server answers the browser, not the Platinum client.
Without a listener at `redirect_uri` the `302` completes in the
browser and the process never sees the code.

## Redirect {#redirect}

`authorizationUrl()` only builds a URI. Something must still collect
`code` and `state` from the redirect. The grant client does not own
a listening server.

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

Two implementations cover the two deployments. A deployed callback
servlet and a loopback listener implement the same receiver
interface.

`CallbackServlet` is a `MapUrl` on an already listening
`Http::Server`. The redirect URI is a public or intranet HTTPS path.
This is the server-side client case. If that URI is a public path on
an already running server, a servlet reads the query.

`LoopbackReceiver` starts a short-lived `Http::Server` on
`127.0.0.1` and a chosen or ephemeral port, maps the redirect path,
and stops listening after the first matching GET (or after a
timeout). The redirect URI looks like
`http://127.0.0.1:7890/callback`. Binding loopback rather than
`0.0.0.0` keeps the callback off the LAN. This is the native-app
case in RFC 8252: a desktop, a command-line tool, or a service with
no public site. Loopback HTTP is allowed only for that native
redirect. HTTPS is required for real deployments.

Client-credentials and device-code grants do not use a receiver.
Custom URI schemes (`myapp://callback`) are a third receiver,
platform-specific, and out of the first implementation.

```
include/Pt/Http/RedirectReceiver.h
```
