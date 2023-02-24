FROM quay.io/almalinuxorg/9-minimal:9

RUN microdnf install -y --nodocs --noplugins epel-release && \
    microdnf install -y --nodocs --noplugins make gcc libubsan libasan inotify-tools && \
    microdnf clean all

WORKDIR /app
COPY . .
