#!/usr/bin/env python3

"""
Sample HTTP server with minimal API to work with the world generator
"""

import sys
import argparse
import http.server


DEFAULT_HOST = "127.0.0.1"
DEFAULT_PORT = 8080


class WebHandler(http.server.BaseHTTPRequestHandler):
    requests = 0
    total_bytes = 0

    def do_POST(self):
        total = remaining = int(self.headers.get("Content-Length", 1024**2))
        while remaining > 0:
            line = self.rfile.readline(remaining)
            remaining -= len(line)
            print(line)

        type(self).requests += 1
        type(self).total_bytes += total

        self.send_response(200)
        self.end_headers()
        self.wfile.write(b"Accepted your upload. Thanks.")
        self.wfile.flush()


def main(argv: list = None):
    parser = argparse.ArgumentParser()
    parser.add_argument("--host", help="host to bind to", default=DEFAULT_HOST)
    parser.add_argument("--port", help="port to bind to", default=DEFAULT_PORT, type=int)

    argv = argv or sys.argv[1:]
    args = parser.parse_args(argv)

    httpd = http.server.HTTPServer((args.host, args.port), WebHandler)
    print(f"Listening on {httpd.server_name} ({httpd.server_address[0]}) port {httpd.server_port} ...")

    try:
        httpd.serve_forever()
    except KeyboardInterrupt:
        print(f"Requests: {WebHandler.requests}\nBody: {WebHandler.total_bytes / 1024:.2f} kB")


if __name__ == "__main__":
    main(sys.argv[1:])
