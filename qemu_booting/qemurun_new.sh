IMAGEFILE=Fedora-Server-Host-Generic-42.20250414-8635a3a5bfcd.riscv64.raw

qemu-system-riscv64 \
  -bios u-boot-spl.bin \
  -nographic \
  -machine virt \
  -smp 4 \
  -m 8G \
  -device loader,file=u-boot.itb,addr=0x80200000 \
  -object rng-random,filename=/dev/urandom,id=rng0 \
  -device virtio-rng-device,rng=rng0 \
  -device virtio-blk-device,drive=hd0 \
  -drive file=${IMAGEFILE},format=raw,id=hd0,if=none \
  -device virtio-net-device,netdev=usernet \
  -netdev user,id=usernet,hostfwd=tcp::10000-:22

