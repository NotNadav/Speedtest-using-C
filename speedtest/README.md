# speedtest

Network speed testing tool. Measures ping, download, and upload speeds.

---

## windows

**what you need:**

- MinGW (gcc for windows)
- that's it

**how to build:**

```
cd windows
make
```

If you don't have make, do this instead:

```
cd windows
gcc -Wall -O2 -mwindows -c main.c
gcc -Wall -O2 -mwindows -c speedtest.c
gcc -Wall -O2 -mwindows -o speedtest.exe main.o speedtest.o -lwinhttp -lws2_32
```

**how to run:**

```
speedtest.exe
```

or just double-click the exe after you build it

---

## linux

**what you need:**

- gcc (you probably have this)
- GTK3
- libcurl

for ubuntu/debian:

```bash
sudo apt-get install build-essential libgtk-3-dev libcurl4-openssl-dev
```

for fedora:

```bash
sudo dnf install gcc gtk3-devel libcurl-devel
```

**how to build:**

```bash
cd linux
make
```

**how to run:**

```bash
./speedtest
```

---

click "Start Test" and wait. that's all there is to it.
