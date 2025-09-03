import serial
import time
import sys
import os
import argparse

# ================== CONFIG (defaults) ==================
DEFAULT_PORT = "COM11"
DEFAULT_BAUD = 115200
DEFAULT_BLOCK_SIZE = 128
# ======================================================

STX = 0x02


def calc_checksum(seq, data):
    c = seq ^ len(data)
    for b in data:
        c ^= b
    return c & 0xFF


def send_packet(ser, seq, data, debug=False):
    pkt = bytearray()
    pkt.append(STX)
    pkt.append(seq & 0xFF)
    pkt.append(len(data) & 0xFF)
    pkt.extend(data)
    pkt.append(calc_checksum(seq, data))
    ser.write(pkt)
    ser.flush()

    start = time.time()
    while time.time() - start < 5:
        resp = ser.readline().decode(errors="ignore").strip()
        if not resp:
            continue
        if debug:
            print(f"[DBG] reply for seq {seq}: '{resp}'")
        if resp == "OK":
            return True
        if resp.startswith("ERR"):
            print(f"[ERR] seq {seq} -> {resp}")
            return False
    print(f"[ERR] timeout waiting for OK for seq {seq}")
    return False


def wait_for_ok(ser, timeout=5, debug=False):
    start = time.time()
    while time.time() - start < timeout:
        resp = ser.readline().decode(errors="ignore").strip()
        if not resp:
            continue
        if debug:
            print(f"[DBG] recv: '{resp}'")
        if resp == "OK":
            return True
    return False


def main():
    parser = argparse.ArgumentParser(description="STM32 firmware uploader")
    parser.add_argument("firmware", help="Path to firmware .bin")
    parser.add_argument("--port", default=DEFAULT_PORT, help="Serial port (default COM11)")
    parser.add_argument("--baud", type=int, default=DEFAULT_BAUD, help="Baud rate")
    parser.add_argument("--block", type=int, default=DEFAULT_BLOCK_SIZE, help="Block size bytes")
    parser.add_argument("--retries", type=int, default=3, help="Number of START attempts")
    parser.add_argument("--timeout", type=int, default=5, help="Timeout seconds for OK reply")
    parser.add_argument("--debug", action="store_true", help="Print debug RX lines")
    parser.add_argument("--dtr-reset", action="store_true", help="Toggle DTR to try to reset target (if wired)")
    args = parser.parse_args()

    fw_path = args.firmware
    if not os.path.exists(fw_path):
        print(f"File not found: {fw_path}")
        return

    with open(fw_path, "rb") as f:
        firmware = f.read()

    ser = serial.Serial(args.port, args.baud, timeout=1)
    time.sleep(1)

    if args.dtr_reset:
        try:
            ser.setDTR(False)
            time.sleep(0.05)
            ser.setDTR(True)
            time.sleep(0.2)
            if args.debug:
                print("[DBG] toggled DTR")
        except Exception as e:
            print(f"[WARN] DTR toggle failed: {e}")

    try:
        ser.reset_input_buffer()
        ser.reset_output_buffer()
    except Exception:
        pass

    print("[*] Sending START...")
    ok = False
    for attempt in range(1, args.retries + 1):
        ser.write(b"START")
        ser.flush()
        if wait_for_ok(ser, timeout=args.timeout, debug=args.debug):
            ok = True
            break
        print(f"[WARN] no OK after START (attempt {attempt}/{args.retries})")
        time.sleep(0.5)
        try:
            ser.reset_input_buffer()
        except Exception:
            pass

    if not ok:
        print("[ERR] Bootloader không phản hồi OK sau START")
        print("Hướng dẫn: đảm bảo board đang ở chế độ bootloader (reset + BOOT0=1 hoặc nhấn nút bootloader).")
        print("Nếu dùng HC-05/BT, hãy đảm bảo module đã reconnect hoặc thử reset/power-cycle board.")
        return

    print("[OK] Bootloader ready")

    seq = 0
    for i in range(0, len(firmware), args.block):
        block = firmware[i:i+args.block]
        if not send_packet(ser, seq, block, debug=args.debug):
            print("Upload failed!")
            return
        print(f"[OK] Sent block {seq}, size {len(block)}")
        seq = (seq + 1) & 0xFF

    print("[*] Sending END...")
    ser.write(b"E")
    ser.flush()
    if wait_for_ok(ser, timeout=2, debug=args.debug):
        print("[Bootloader]: OK")
    else:
        print("[Bootloader]: No response")

    print("Firmware upload complete!")


if __name__ == "__main__":
    main()
