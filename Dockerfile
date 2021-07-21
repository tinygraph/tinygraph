FROM alpine:3.14

RUN apk add --no-cache make gcc musl-dev inotify-tools

WORKDIR /app
COPY . .

CMD ["make", "watch"]
