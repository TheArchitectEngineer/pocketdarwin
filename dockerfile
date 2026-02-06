FROM archlinux:multilib-devel-20260201.0.486523

WORKDIR /app

RUN pacman -Sy \
    pacman -Syu

RUN pacman -S gcc git
RUN git clone https://aur.archlinux.org/yay-bin.git
RUN cd yay-bin \
    makepkg -si


