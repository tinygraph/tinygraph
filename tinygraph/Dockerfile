FROM quay.io/almalinuxorg/9-minimal:9

RUN microdnf install -y --nodocs --noplugins epel-release && \
    microdnf install -y --nodocs --noplugins make gcc inotify-tools && \
    microdnf clean all

WORKDIR /app
COPY . .

CMD ["make", "watch"]
