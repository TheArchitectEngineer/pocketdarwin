FROM archlinux:latest

ENV LANG=C.UTF-8
ENV LC_ALL=C.UTF-8

# Update and install kernel build dependencies
RUN pacman -Syu --noconfirm && \
    pacman -S --noconfirm \
        base-devel \
        git \
        bc \
        flex \
        bison \
        ncurses \
        openssl \
        elfutils \
        pahole \
        cpio \
        rsync \
        wget \
        python \
        python-setuptools \
        python-wheel \
        perl \
        llvm \
        clang \
        lld \
        cmake \
        ninja \
        util-linux \
        kmod \
        sudo \
        which \
        file \
        vim \
        aarch64-linux-gnu-gcc \
        nano && \
    pacman -Scc --noconfirm

# Non-root user (kernel builds prefer this)
RUN useradd -m builder && \
    echo "builder ALL=(ALL) NOPASSWD: ALL" >> /etc/sudoers

USER builder
WORKDIR /home/builder

# Sensible defaults for Android / ARM64 kernels
ENV ARCH=arm64
ENV LLVM=1
ENV LLVM_IAS=1
ENV CC=clang
ENV LD=ld.lld

CMD ["/bin/bash"]
