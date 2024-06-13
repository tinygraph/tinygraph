FROM almalinux:9.4-minimal

RUN microdnf install -y --refresh --best --nodocs --noplugins --setopt=install_weak_deps=0 epel-release && \
    microdnf install -y --refresh --best --nodocs --noplugins --setopt=install_weak_deps=0 make clang libubsan libasan inotify-tools && \
    microdnf clean all

WORKDIR /app
COPY . .
