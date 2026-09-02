---
description: "HTTP Basics, Messages and Streams"
---

- Parse and iterate HTTP headers, access message fields and body:
  `include/Pt/Http/Message.h`
- Create HTTP requests, set method and URL, access query parameters:
  `include/Pt/Http/Request.h`
- Create HTTP replies, set status code and status text:
  `include/Pt/Http/Reply.h`
- Read and write HTTP messages asynchronously with input and output signals:
  `include/Pt/Http/IOStream.h`
- Handle HTTP I/O errors:
  `include/Pt/Http/HttpError.h`
- Store user names and passwords for HTTP authentication and authorization:
  `include/Pt/Http/Credentials.h`