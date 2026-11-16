# Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
# Chapter 14: Testing Real-Time Systems
# 14.6 Hardware-in-the-Loop (HIL) Testing > Scripted Test Sequences
# Verification: not run (host command / other RTOS / configuration)
# SPDX-License-Identifier: MIT
# hil_tests/test_temperature_alarm.py
import time
import pyvisa
import serial

def test_over_temperature_alarm(dac, target_uart):
    """Ramp simulated temperature above alarm threshold,
       verify target raises alarm within 500 ms."""
    # Set simulated temperature to 20 °C (0.5V)
    dac.write(":VOLT 0.5")
    time.sleep(1.0)

    # Ramp to 90 °C (2.25V) — above 85 °C alarm threshold
    dac.write(":VOLT 2.25")
    start = time.time()

    # Wait for alarm message on UART
    target_uart.timeout = 2.0
    response = target_uart.readline().decode()
    elapsed = time.time() - start

    assert "ALARM:OVER_TEMP" in response, \
        f"Expected alarm, got: {response}"
    assert elapsed < 0.5, \
        f"Alarm latency {elapsed:.3f}s exceeds 500ms requirement"
