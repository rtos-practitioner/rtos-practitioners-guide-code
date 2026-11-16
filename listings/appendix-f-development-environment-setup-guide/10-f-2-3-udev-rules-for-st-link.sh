# Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
# Appendix F: Development Environment Setup Guide
# F.2 Linux Setup (Ubuntu 22.04+) > F.2.3 Udev Rules for ST-Link
# Verification: not run (host command / other RTOS / configuration)
# SPDX-License-Identifier: MIT
sudo tee /etc/udev/rules.d/70-st-link.rules > /dev/null <<'EOF'
# STMicroelectronics ST-Link V2
ATTRS{idVendor}=="0483", ATTRS{idProduct}=="3748", MODE="0666", GROUP="plugdev"
# STMicroelectronics ST-Link V2-1
ATTRS{idVendor}=="0483", ATTRS{idProduct}=="374b", MODE="0666", GROUP="plugdev"
# STMicroelectronics ST-Link V3
ATTRS{idVendor}=="0483", ATTRS{idProduct}=="374e", MODE="0666", GROUP="plugdev"
ATTRS{idVendor}=="0483", ATTRS{idProduct}=="374f", MODE="0666", GROUP="plugdev"
EOF

sudo udevadm control --reload-rules
sudo udevadm trigger
