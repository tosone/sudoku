FROM alpine:edge

RUN sed -i 's/http:\/\/dl-cdn.alpinelinux.org/https:\/\/mirrors.tuna.tsinghua.edu.cn/g' /etc/apk/repositories && \
  apk update && apk upgrade \
  && apk add --no-cache build-base mdocml-apropos coreutils ca-certificates \
  vim fish dialog tzdata ncurses-dev

RUN sed -i "s/bin\/ash/usr\/bin\/fish/" /etc/passwd

RUN echo "set mouse-=a" >> ~/.vimrc

RUN rm -f /etc/localtime && ln -s /usr/share/zoneinfo/Asia/Shanghai /etc/localtime

ENV SHELL /usr/bin/fish

VOLUME /sudoku

WORKDIR /sudoku

CMD ["/usr/bin/fish"]
