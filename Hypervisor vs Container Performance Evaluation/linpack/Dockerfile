FROM gcc:4.8.5 AS build
WORKDIR /app
COPY . .
RUN gcc -O1 linpack.c -o linpack -static

FROM scratch
COPY --from=build /app/linpack /
CMD ["/linpack"]
