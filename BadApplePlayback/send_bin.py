# send_bin.py
# Usage: python send_bin.py COM5 badapple_12x9_30fps_8bit.bin

import sys, struct, serial, time, os

def main():
    if len(sys.argv) < 3:
        print("Usage: python send_bin.py COM5 badapple_12x9_30fps_8bit.bin")
        sys.exit(1)

    port = sys.argv[1]
    path = sys.argv[2]

    if not os.path.exists(path):
        print("File not found:", path)
        sys.exit(1)

    data = open(path, "rb").read()
    print("Bytes:", len(data))

    # Longer timeout so we don't exit during long verify phase
    ser = serial.Serial(port, 921600, timeout=10)
    time.sleep(2.0)  # 32U4 often resets when serial opens

    # Send length (uint32 little-endian) then raw data
    ser.write(struct.pack("<I", len(data)))
    ser.write(data)
    ser.flush()

    # Read lines until DONE (or safety timeout)
    t0 = time.time()
    while time.time() - t0 < 300:  # 5 minutes max
        line = ser.readline()
        if not line:
            continue
        s = line.decode(errors="ignore").strip()
        if s:
            print(s)
        if "DONE" in s:
            break

    ser.close()

if __name__ == "__main__":
    main()
