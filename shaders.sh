for f in res/shaders/*.vert res/shaders/*.frag; do
    echo "Compiling: $f"
    /usr/bin/glslc "$f" -o "${f%}.spv"
done