FROM alpine:3.13 as BUILD

RUN sed -i 's/https:\/\/dl-cdn.alpinelinux.org/https:\/\/mirrors.tuna.tsinghua.edu.cn/g' /etc/apk/repositories && \
  apk add --no-cache build-base ncurses-dev

WORKDIR /sudoku

COPY . .

RUN make && cp sudoku.test puzzles.txt /tmp

CMD ["/bin/sh"]

FROM alpine:3.13

RUN sed -i 's/https:\/\/dl-cdn.alpinelinux.org/https:\/\/mirrors.tuna.tsinghua.edu.cn/g' /etc/apk/repositories && \
  apk add --no-cache ncurses

COPY --from=BUILD /tmp/sudoku.test /usr/local/bin

WORKDIR /sudoku
COPY --from=BUILD /tmp/puzzles.txt .

CMD ["/bin/sh", "-c", "while :; do cat puzzles.txt | sudoku.test; sleep 5; done"]
