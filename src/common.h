/*! \file common.h
 *  \brief Auto-generated file. Do not modify.
 *  \author Georgi Gerganov
 */

#pragma once

// the main js module
constexpr auto kImGuiWS_js = R"js(

var imgui_ws = {
    canvas: null,
    gl: null,

    shader_program: null,
    vertex_buffer: null,
    index_buffer: null,

    attribute_location_tex: null,
    attribute_location_proj_mtx: null,
    attribute_location_position: null,
    attribute_location_uv: null,
    attribute_location_color: null,

    tex_font_id: null,

    io: {
        mouse_x: 0.0,
        mouse_y: 0.0,
        mouse_left_down: 0,

        want_capture_mouse: true,
    },

    init: function(canvas_name) {
        this.canvas = document.getElementById(canvas_name);
        this.gl = this.canvas.getContext('webgl');

        var onpointermove = this.canvas_on_pointermove.bind(this);
        var onpointerdown = this.canvas_on_pointerdown.bind(this);
        var onpointerup = this.canvas_on_pointerup.bind(this);

        this.canvas.style.touchAction = "none"; // Disable browser handling of all panning and zooming gestures.
        //this.canvas.addEventListener("blur", this.canvas_on_blur);
        //this.canvas.addEventListener("keydown", this.canvas_on_keydown);
        //this.canvas.addEventListener("keyup", this.canvas_on_keyup);
        //this.canvas.addEventListener("keypress", this.canvas_on_keypress);
        this.canvas.addEventListener("pointermove", onpointermove);
        this.canvas.addEventListener("pointerdown", onpointerdown);
        this.canvas.addEventListener("pointerup", onpointerup);
        //this.canvas.addEventListener("contextmenu", this.canvas_on_contextmenu);
        //this.canvas.addEventListener("wheel", this.canvas_on_wheel);

        this.vertex_buffer = this.gl.createBuffer();
        this.index_buffer = this.gl.createBuffer();

        var vertex_shader_source =
            'uniform mat4 ProjMtx;' +
            'attribute vec2 Position;' +
            'attribute vec2 UV;' +
            'attribute vec4 Color;' +
            'varying vec2 Frag_UV;' +
            'varying vec4 Frag_Color;' +
            'void main(void) {' +
            '	Frag_UV = UV;' +
            '	Frag_Color = Color;' +
            '   gl_Position = ProjMtx * vec4(Position, 0.0, 1.0);' +
            '}';

        var vertex_shader = this.gl.createShader(this.gl.VERTEX_SHADER);
        this.gl.shaderSource(vertex_shader, vertex_shader_source);
        this.gl.compileShader(vertex_shader);

        var fragment_shader_source = [
            'precision mediump float;' +
            'uniform sampler2D Texture;' +
            'varying vec2 Frag_UV;' +
            'varying vec4 Frag_Color;' +
            'void main() {' +
            '	gl_FragColor = Frag_Color * texture2D(Texture, Frag_UV);' +
            '}'
        ];

        var fragment_shader = this.gl.createShader(this.gl.FRAGMENT_SHADER);
        this.gl.shaderSource(fragment_shader, fragment_shader_source);
        this.gl.compileShader(fragment_shader);

        this.shader_program = this.gl.createProgram();
        this.gl.attachShader(this.shader_program, vertex_shader);
        this.gl.attachShader(this.shader_program, fragment_shader);
        this.gl.linkProgram(this.shader_program);

        this.attribute_location_tex      = this.gl.getUniformLocation(this.shader_program,   "Texture");
        this.attribute_location_proj_mtx = this.gl.getUniformLocation(this.shader_program,   "ProjMtx");
        this.attribute_location_position = this.gl.getAttribLocation(this.shader_program,    "Position");
        this.attribute_location_uv       = this.gl.getAttribLocation(this.shader_program,    "UV");
        this.attribute_location_color    = this.gl.getAttribLocation(this.shader_program,    "Color");
    },

    init_font: function(tex_font_abuf) {
        var tex_font_uint8 = new Uint8Array(tex_font_abuf);
        var tex_font_int32 = new Int32Array(tex_font_abuf);

        const width = tex_font_int32[2];
        const height = tex_font_int32[3];

        var pixels = new Uint8Array(4*width*height);
        for (var i = 0; i < width*height; ++i) {
            pixels[4*i + 3] = tex_font_uint8[16 + i];
            pixels[4*i + 2] = 0xFF;
            pixels[4*i + 1] = 0xFF;
            pixels[4*i + 0] = 0xFF;
        }

        this.tex_font_id = this.gl.createTexture();
        this.gl.bindTexture(this.gl.TEXTURE_2D, this.tex_font_id);
        this.gl.texParameteri(this.gl.TEXTURE_2D, this.gl.TEXTURE_MIN_FILTER, this.gl.LINEAR);
        this.gl.texParameteri(this.gl.TEXTURE_2D, this.gl.TEXTURE_MAG_FILTER, this.gl.LINEAR);
        //this.gl.pixelStorei(gl.UNPACK_ROW_LENGTH); // WebGL2
        this.gl.texImage2D(this.gl.TEXTURE_2D, 0, this.gl.RGBA, width, height, 0, this.gl.RGBA, this.gl.UNSIGNED_BYTE, pixels);
    },

    render: function(draw_data_abuf) {
        // Backup GL state
        const last_active_texture = this.gl.getParameter(this.gl.ACTIVE_TEXTURE) || null;
        const last_program = this.gl.getParameter(this.gl.CURRENT_PROGRAM) || null;
        const last_texture = this.gl.getParameter(this.gl.TEXTURE_BINDING_2D) || null;
        const last_array_buffer = this.gl.getParameter(this.gl.ARRAY_BUFFER_BINDING) || null;
        const last_element_array_buffer = this.gl.getParameter(this.gl.ELEMENT_ARRAY_BUFFER_BINDING) || null;
        const last_viewport = this.gl.getParameter(this.gl.VIEWPORT) || null;
        const last_scissor_box = this.gl.getParameter(this.gl.SCISSOR_BOX) || null;
        const last_blend_src_rgb = this.gl.getParameter(this.gl.BLEND_SRC_RGB) || null;
        const last_blend_dst_rgb = this.gl.getParameter(this.gl.BLEND_DST_RGB) || null;
        const last_blend_src_alpha = this.gl.getParameter(this.gl.BLEND_SRC_ALPHA) || null;
        const last_blend_dst_alpha = this.gl.getParameter(this.gl.BLEND_DST_ALPHA) || null;
        const last_blend_equation_rgb = this.gl.getParameter(this.gl.BLEND_EQUATION_RGB) || null;
        const last_blend_equation_alpha = this.gl.getParameter(this.gl.BLEND_EQUATION_ALPHA) || null;
        const last_enable_blend = this.gl.getParameter(this.gl.BLEND) || null;
        const last_enable_cull_face = this.gl.getParameter(this.gl.CULL_FACE) || null;
        const last_enable_depth_test = this.gl.getParameter(this.gl.DEPTH_TEST) || null;
        const last_enable_scissor_test = this.gl.getParameter(this.gl.SCISSOR_TEST) || null;

        var draw_data_offset = 0;
        var draw_data_uint8 = new Uint8Array(draw_data_abuf);
        var draw_data_uint16 = new Uint16Array(draw_data_abuf);
        var draw_data_uint32 = new Uint32Array(draw_data_abuf);
        var draw_data_float = new Float32Array(draw_data_abuf);

        this.gl.enable(this.gl.BLEND);
        this.gl.blendEquation(this.gl.FUNC_ADD);
        this.gl.blendFunc(this.gl.SRC_ALPHA, this.gl.ONE_MINUS_SRC_ALPHA);
        this.gl.disable(this.gl.CULL_FACE);
        this.gl.disable(this.gl.DEPTH_TEST);
        this.gl.enable(this.gl.SCISSOR_TEST);
        this.gl.viewport(0, 0, this.canvas.width, this.canvas.height);

        var clip_off_x = 0.0;
        var clip_off_y = 0.0;

        const L = clip_off_x;
        const R = clip_off_x + this.canvas.width;
        const T = clip_off_y;
        const B = clip_off_y + this.canvas.height;

        const ortho_projection = new Float32Array([
            2.0 / (R - L), 0.0, 0.0, 0.0,
            0.0, 2.0 / (T - B), 0.0, 0.0,
            0.0, 0.0, -1.0, 0.0,
            (R + L) / (L - R), (T + B) / (B - T), 0.0, 1.0,
        ]);
        this.gl.useProgram(this.shader_program);
        this.gl.uniform1i(this.attribute_location_tex, 0);
        this.gl.uniformMatrix4fv(this.attribute_location_proj_mtx, false, ortho_projection);

        this.gl.bindBuffer(this.gl.ARRAY_BUFFER, this.vertex_buffer);

        this.gl.enableVertexAttribArray(this.attribute_location_position);
        this.gl.enableVertexAttribArray(this.attribute_location_uv);
        this.gl.enableVertexAttribArray(this.attribute_location_color);
        this.gl.vertexAttribPointer(this.attribute_location_position, 2, this.gl.FLOAT,         false, 5*4, 0);
        this.gl.vertexAttribPointer(this.attribute_location_uv,       2, this.gl.FLOAT,         false, 5*4, 2*4);
        this.gl.vertexAttribPointer(this.attribute_location_color,    4, this.gl.UNSIGNED_BYTE, true,  5*4, 4*4);

        var p = null;
        var n_cmd_lists = draw_data_uint32[draw_data_offset]; draw_data_offset += 4;
        for (var i_list = 0; i_list < n_cmd_lists; ++i_list) {
            p = new Uint32Array(draw_data_abuf, draw_data_offset, 1);
            var n_vertices = p[0]; draw_data_offset += 4;

            var av = new Float32Array(draw_data_abuf, draw_data_offset, 5*n_vertices);
            this.gl.bindBuffer(this.gl.ARRAY_BUFFER, this.vertex_buffer);
            this.gl.bufferData(this.gl.ARRAY_BUFFER, av, this.gl.STREAM_DRAW);
            draw_data_offset += 5*4*n_vertices;

            p = new Uint32Array(draw_data_abuf, draw_data_offset, 1);
            var n_indices = p[0]; draw_data_offset += 4;

            var ai = new Uint16Array(draw_data_abuf, draw_data_offset, n_indices);
            this.gl.bindBuffer(this.gl.ELEMENT_ARRAY_BUFFER, this.index_buffer);
            this.gl.bufferData(this.gl.ELEMENT_ARRAY_BUFFER, ai, this.gl.STREAM_DRAW);
            draw_data_offset += 2*n_indices;

            p = new Uint32Array(draw_data_abuf, draw_data_offset, 1);
            var n_cmd = p[0]; draw_data_offset += 4;

            for (var i_cmd = 0; i_cmd < n_cmd; ++i_cmd) {
                var pi = new Uint32Array(draw_data_abuf, draw_data_offset, 4);
                var n_elements = pi[0]; draw_data_offset += 4;
                var texture_id = pi[1]; draw_data_offset += 4;
                var offset_vtx = pi[2]; draw_data_offset += 4;
                var offset_idx = pi[3]; draw_data_offset += 4;

                var pf = new Float32Array(draw_data_abuf, draw_data_offset, 4);
                var clip_x = pf[0] - clip_off_x; draw_data_offset += 4;
                var clip_y = pf[1] - clip_off_y; draw_data_offset += 4;
                var clip_z = pf[2] - clip_off_x; draw_data_offset += 4;
                var clip_w = pf[3] - clip_off_y; draw_data_offset += 4;

                if (clip_x < this.canvas.width && clip_y < this.canvas.height && clip_z >= 0.0 && clip_w >= 0.0) {
                    this.gl.scissor(clip_x, this.canvas.height - clip_w, clip_z - clip_x, clip_w - clip_y);
                    // todo : temp hack
                    if (texture_id == 1) {
                        this.gl.activeTexture(this.gl.TEXTURE0);
                        this.gl.bindTexture(this.gl.TEXTURE_2D, this.tex_font_id);
                    }
                    this.gl.drawElements(this.gl.TRIANGLES, n_elements, this.gl.UNSIGNED_SHORT, 2*offset_idx);
                }
            }
        }

        (last_program !== null) && this.gl.useProgram(last_program);
        (last_texture !== null) && this.gl.bindTexture(this.gl.TEXTURE_2D, last_texture);
        (last_active_texture !== null) && this.gl.activeTexture(last_active_texture);
        this.gl.disableVertexAttribArray(this.attribute_location_position);
        this.gl.disableVertexAttribArray(this.attribute_location_uv);
        this.gl.disableVertexAttribArray(this.attribute_location_color);
        (last_array_buffer !== null) && this.gl.bindBuffer(this.gl.ARRAY_BUFFER, last_array_buffer);
        (last_element_array_buffer !== null) && this.gl.bindBuffer(this.gl.ELEMENT_ARRAY_BUFFER, last_element_array_buffer);
        (last_blend_equation_rgb !== null && last_blend_equation_alpha !== null) && this.gl.blendEquationSeparate(last_blend_equation_rgb, last_blend_equation_alpha);
        (last_blend_src_rgb !== null && last_blend_src_alpha !== null && last_blend_dst_rgb !== null && last_blend_dst_alpha !== null) && this.gl.blendFuncSeparate(last_blend_src_rgb, last_blend_src_alpha, last_blend_dst_rgb, last_blend_dst_alpha);
        (last_enable_blend ? this.gl.enable(this.gl.BLEND) : this.gl.disable(this.gl.BLEND));
        (last_enable_cull_face ? this.gl.enable(this.gl.CULL_FACE) : this.gl.disable(this.gl.CULL_FACE));
        (last_enable_depth_test ? this.gl.enable(this.gl.DEPTH_TEST) : this.gl.disable(this.gl.DEPTH_TEST));
        (last_enable_scissor_test ? this.gl.enable(this.gl.SCISSOR_TEST) : this.gl.disable(this.gl.SCISSOR_TEST));
        (last_viewport !== null) && this.gl.viewport(last_viewport[0], last_viewport[1], last_viewport[2], last_viewport[3]);
        (last_scissor_box !== null) && this.gl.scissor(last_scissor_box[0], last_scissor_box[1], last_scissor_box[2], last_scissor_box[3]);
    },

    canvas_on_pointermove: function(event) {},
    canvas_on_pointerdown: function(event) {},
    canvas_on_pointerup: function(event) {},
}


)js";
