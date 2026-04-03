mkdir -p release/shaders

cp build/qosmos.exe release/

cp src/shaders/vertex.glsl release/shaders/
cp src/shaders/fragment.glsl release/shaders/
cp src/shaders/vertex_axes.glsl release/shaders/
cp src/shaders/fragment_axes.glsl release/shaders/

ldd build/qosmos.exe \
    | grep ucrt64 \
    | awk '{print $3}' \
    | xargs -I{} cp {} release/