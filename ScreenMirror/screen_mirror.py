#!/usr/bin/env python3
"""
Screen Mirror to LED Matrix - Python Side
Captures screen region, downsamples to 12x9 grayscale, sends to Arduino

Requirements:
    pip install pillow mss pyserial numpy

Usage:
    python screen_mirror.py COM10
    (Replace COM10 with your Arduino's port)
"""

import sys
import time
import struct
import numpy as np
from mss import mss
from PIL import Image
import serial

# Configuration
MATRIX_WIDTH = 12
MATRIX_HEIGHT = 9
FRAME_SIZE = MATRIX_WIDTH * MATRIX_HEIGHT
START_BYTE = 0xFF
TARGET_FPS = 30
FRAME_DELAY = 0.1 / TARGET_FPS

# Screen capture region (adjust these!)
# None = full screen, or specify (left, top, width, height)
CAPTURE_REGION = None  # Captures entire screen
#CAPTURE_REGION = {'left': 0, 'top': 0, 'width': 800, 'height': 600}  # Example region
#CAPTURE_REGION = {'left': 0, 'top': 0, 'width': 960, 'height': 540}  # Example region, top quadrant


def downsample_to_matrix(screenshot):
    """
    Downsample screenshot to 12x9 grayscale matrix
    
    Args:
        screenshot: PIL Image
        
    Returns:
        list of 108 bytes (brightness values 0-255)
    """
    # Convert to grayscale
    grayscale = screenshot.convert('L')
    
    # Resize to 12x9 using high-quality downsampling
    resized = grayscale.resize((MATRIX_WIDTH, MATRIX_HEIGHT), Image.LANCZOS)
    
    # Convert to numpy array
    pixels = np.array(resized)
    
    # Flatten to 1D array (row-major order)
    frame_data = pixels.flatten()
    
    return frame_data.tolist()


def send_frame(ser, frame_data):
    """
    Send frame data to Arduino via Serial
    
    Protocol:
        - Start byte: 0xFF
        - 108 bytes of pixel data
    """
    # Create packet
    packet = bytes([START_BYTE]) + bytes(frame_data)
    
    # Send to Arduino
    ser.write(packet)
    ser.flush()


def main():
    if len(sys.argv) < 2:
        print("Usage: python screen_mirror.py <COM_PORT>")
        print("Example: python screen_mirror.py COM10")
        print("         python screen_mirror.py /dev/ttyACM0  (Linux/Mac)")
        sys.exit(1)
    
    port = sys.argv[1]
    
    print("=" * 50)
    print("Screen Mirror to LED Matrix")
    print("=" * 50)
    print(f"Target: {MATRIX_WIDTH}x{MATRIX_HEIGHT} LED matrix")
    print(f"Port: {port}")
    print(f"Baud: 115200")
    print(f"Target FPS: {TARGET_FPS}")
    print()
    
    # Open serial connection
    try:
        ser = serial.Serial(port, 115200, timeout=1)
        time.sleep(2)  # Wait for Arduino to initialize
        print("✓ Connected to Arduino")
    except Exception as e:
        print(f"✗ Failed to connect: {e}")
        sys.exit(1)
    
    # Setup screen capture
    sct = mss()
    
    # Determine capture region
    if CAPTURE_REGION is None:
        # Capture entire primary monitor
        monitor = sct.monitors[1]  # Monitor 1 is primary
        capture_region = {
            'left': monitor['left'],
            'top': monitor['top'],
            'width': monitor['width'],
            'height': monitor['height']
        }
        print(f"✓ Capturing full screen: {monitor['width']}x{monitor['height']}")
    else:
        capture_region = CAPTURE_REGION
        print(f"✓ Capturing region: {capture_region['width']}x{capture_region['height']}")
    
    print()
    print("Starting screen mirror...")
    print("Press Ctrl+C to stop")
    print()
    
    frame_count = 0
    start_time = time.time()
    fps_timer = time.time()
    fps_counter = 0
    
    try:
        while True:
            loop_start = time.time()
            
            # Capture screen
            screenshot = sct.grab(capture_region)
            img = Image.frombytes('RGB', screenshot.size, screenshot.rgb)
            
            # Downsample to 12x9
            frame_data = downsample_to_matrix(img)
            
            # Send to Arduino
            send_frame(ser, frame_data)
            
            # Statistics
            frame_count += 1
            fps_counter += 1
            
            # Print FPS every second
            if time.time() - fps_timer >= 1.0:
                print(f"FPS: {fps_counter:.1f} | Frames sent: {frame_count}")
                fps_counter = 0
                fps_timer = time.time()
            
            # Frame rate limiting
            elapsed = time.time() - loop_start
            if elapsed < FRAME_DELAY:
                time.sleep(FRAME_DELAY - elapsed)
    
    except KeyboardInterrupt:
        print("\n")
        print("=" * 50)
        print("Stopped by user")
        elapsed_time = time.time() - start_time
        avg_fps = frame_count / elapsed_time if elapsed_time > 0 else 0
        print(f"Total frames: {frame_count}")
        print(f"Average FPS: {avg_fps:.1f}")
        print("=" * 50)
    
    finally:
        ser.close()
        print("Serial connection closed")


if __name__ == "__main__":
    main()
