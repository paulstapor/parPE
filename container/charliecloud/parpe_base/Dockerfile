FROM ubuntu:19.04

COPY . /u18

RUN /u18/install.sh && rm -rf /tmp && mkdir /tmp

ENV BASH_ENV "/etc/drydock/.env"

RUN /u18/install_parpe.sh
