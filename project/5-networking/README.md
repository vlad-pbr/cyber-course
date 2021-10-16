# 5 - Networking

Task: create UDP client and server, send a magic string from client to server and capture it mid-way to see the unencrypted password.

## Intuition
UDP packets are not encrypted in any way, meaning a man in the middle can intercept and read the packets as plaintext.
In our case, client and server are on the same machine and they communicate over localhost.
We use `tcpdump` to intercept and read the packet.

## Step-by-Step

1. Start capturing UDP packets on loopback device:
``` bash
make capture
```

2. Using a separate terminal, start server, then client and send a packet containing the magic string:
``` bash
make magic_string=admin_123456 run
```

3. Go back to the previous terminal and inspect the result.