FROM alpine:3.12
LABEL description="Container to build and serve neoid"

ENV PROJECT_DIR=/app

WORKDIR $PROJECT_DIR
# copy neoid source
COPY . $PROJECT_DIR

# build 
RUN apk add build-base && make

# expose port
ENV APP_PORT=18989
expose $APP_PORT
HEALTHCHECK --interval=30s --timeout=30s \
  CMD curl -fs http://localhost:$APP_PORT || exit 1

ENTRYPOINT ["build/bin/neoid", "-i", "$machine_id", "-b", "1024"]

