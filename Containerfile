FROM alpine

RUN apk add --no-cache yarn make g++ python3

WORKDIR /work
COPY package.json index.js .
COPY poll poll
COPY interrupt interrupt
RUN yarn install

ENTRYPOINT ["/usr/bin/yarn", "start"]
