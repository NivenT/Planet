#include "utils.h"

using namespace glm;
using namespace nta;

void render_line_in_pieces(PrimitiveBatch& pbatch, vec2 start, vec2 end, int num_pieces, vec4 color) {
    const vec2 diff = end - start;
    for (int i = 0; i < num_pieces; i++) {
        pbatch.addPrimitive({{start + (float)i*diff/(float)num_pieces, color},
                             {start + (float)(i+1)*diff/(float)num_pieces, color}});
    }
}