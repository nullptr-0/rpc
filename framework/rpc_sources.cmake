include(crypto_sources.cmake)
include(rpcfunc_sources.cmake)

set(RPC_SERVER_SOURCES
    ${CRYPTO_SOURCES}
    ${RPC_FUNC_SOURCES}
    rpc-server.cpp
    rpc-server-impl.cpp
    ${RPC_CUSTOM_DEF_SOURCES}
)

set(RPC_SERVER_HEADERS
    ${CRYPTO_HEADERS}
    crypto/ecc.hpp
    json.hpp
    rpc-custom-type-serializer.hpp
    rpc-server.hpp
    descman.hpp
    unisock.hpp
)

set(RPC_CLIENT_SOURCES
    ${CRYPTO_SOURCES}
    ${RPC_CLIENT_FUNCCALL_SOURCES}
    rpc-client.cpp
    rpc-client-impl.cpp
    ${RPC_CUSTOM_DEF_SOURCES}
)

set(RPC_CLIENT_HEADERS
    ${CRYPTO_HEADERS}
    crypto/ecc.hpp
    json.hpp
    rpc-custom-type-serializer.hpp
    rpc-client.hpp
    rpc-function.hpp
    descman.hpp
    unisock.hpp
)
