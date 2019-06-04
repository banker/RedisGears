ARG OSNICK=bionic

#----------------------------------------------------------------------------------------------
FROM redislabs/redis-${OSNICK}:latest AS builder

ADD . /build
WORKDIR /build

RUN ./deps/readies/bin/getpy2
RUN python system-setup.py
RUN make get_deps
RUN make all SHOW=1 PYTHON_ENCODING=ucs4

#----------------------------------------------------------------------------------------------
FROM redislabs/redis-${OSNICK}:latest

ENV REDIS_MODULES /opt/redislabs/lib/modules

RUN mkdir -p $REDIS_MODULES/

COPY --from=builder /build/redisgears.so $REDIS_MODULES/
COPY --from=builder /build/artifacts/release/redisgears-dependencies.*.tgz /tmp/

RUN tar xzf /tmp/redisgears-dependencies.*.tgz -C /

CMD ["--loadmodule", "/opt/redislabs/lib/modules/redisgears.so", "PythonHomeDir", "/opt/redislabs/lib/modules/python3"]
