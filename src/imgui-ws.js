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

    device_pixel_ratio: window.device_pixel_ratio || 1,

    tex_map_id: {},
    tex_map_rev: {},
    tex_map_abuf: {},

    n_draw_lists: null,
    draw_lists_abuf: {},

    io: {
        mouse_x: 0.0,
        mouse_y: 0.0,

        want_capture_mouse: true,
        want_capture_keyboard: true,
    },

    init: function(canvas_name) {
        this.canvas = document.getElementById(canvas_name);

        var onkeyup = this.canvas_on_keyup.bind(this);
        var onkeydown = this.canvas_on_keydown.bind(this);
        var onkeypress = this.canvas_on_keypress.bind(this);
        var onpointermove = this.canvas_on_pointermove.bind(this);
        var onpointerdown = this.canvas_on_pointerdown.bind(this);
        var onpointerup = this.canvas_on_pointerup.bind(this);
        var onwheel = this.canvas_on_wheel.bind(this);
        var ontouch = this.canvas_on_touch.bind(this);

        //this.canvas.style.touchAction = 'none'; // Disable browser handling of all panning and zooming gestures.
        //this.canvas.addEventListener('blur', this.canvas_on_blur);

        this.canvas.addEventListener('keyup', onkeyup, true);
        this.canvas.addEventListener('keydown', onkeydown, true);
        this.canvas.addEventListener('keypress', onkeypress, true);

        this.canvas.addEventListener('pointermove', onpointermove);
        this.canvas.addEventListener('mousemove', onpointermove);

        this.canvas.addEventListener('pointerdown', onpointerdown);
        this.canvas.addEventListener('mousedown', onpointerdown);

        this.canvas.addEventListener('pointerup', onpointerup);
        this.canvas.addEventListener('mouseup', onpointerup);

        this.canvas.addEventListener('contextmenu', function(event) { event.preventDefault(); });
        this.canvas.addEventListener('wheel', onwheel, false);

        this.canvas.addEventListener('touchstart', ontouch);
        this.canvas.addEventListener('touchmove', ontouch);
        this.canvas.addEventListener('touchend', ontouch);
        this.canvas.addEventListener('touchcancel', ontouch);

        this.gl = this.canvas.getContext('webgl');

        this.vertex_buffer = this.gl.createBuffer();
        this.index_buffer = this.gl.createBuffer();

        var vertex_shader_source = [
            'precision mediump float;' +
            'uniform mat4 ProjMtx;' +
            'attribute vec2 Position;' +
            'attribute vec2 UV;' +
            'attribute vec4 Color;' +
            'varying vec2 Frag_UV;' +
            'varying vec4 Frag_Color;' +
            'void main(void) {' +
            '	Frag_UV = UV;' +
            '	Frag_Color = Color;' +
            '   gl_Position = ProjMtx * vec4(Position, 0, 1);' +
            '}'
        ];

        var vertex_shader = this.gl.createShader(this.gl.VERTEX_SHADER);
        this.gl.shaderSource(vertex_shader, vertex_shader_source);
        this.gl.compileShader(vertex_shader);

        var fragment_shader_source = [
            'precision mediump float;' +
            'uniform sampler2D Texture;' +
            'varying vec2 Frag_UV;' +
            'varying vec4 Frag_Color;' +
            'void main() {' +
            '	gl_FragColor = Frag_Color * texture2D(Texture, Frag_UV.st);' +
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

        this.gl.enable(this.gl.BLEND);
        this.gl.blendEquation(this.gl.FUNC_ADD);
        this.gl.blendFunc(this.gl.SRC_ALPHA, this.gl.ONE_MINUS_SRC_ALPHA);
        this.gl.disable(this.gl.CULL_FACE);
        this.gl.disable(this.gl.DEPTH_TEST);
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
        this.gl.bindBuffer(this.gl.ELEMENT_ARRAY_BUFFER, this.index_buffer);

        this.gl.enableVertexAttribArray(this.attribute_location_position);
        this.gl.enableVertexAttribArray(this.attribute_location_uv);
        this.gl.enableVertexAttribArray(this.attribute_location_color);
        this.gl.vertexAttribPointer(this.attribute_location_position, 2, this.gl.FLOAT,         false, 5*4, 0);
        this.gl.vertexAttribPointer(this.attribute_location_uv,       2, this.gl.FLOAT,         false, 5*4, 2*4);
        this.gl.vertexAttribPointer(this.attribute_location_color,    4, this.gl.UNSIGNED_BYTE, true,  5*4, 4*4);
    },

    incppect_textures: function(incppect) {
        var n_textures = incppect.get_int32('imgui.n_textures');

        for (var i = 0; i < n_textures; ++i) {
            var tex_id = incppect.get_int32('imgui.texture_id[%d]', i);
            var tex_rev = incppect.get_int32('imgui.texture_revision[%d]', tex_id);

            if (this.tex_map_abuf[tex_id] == null || this.tex_map_abuf[tex_id].byteLength < 1) {
                this.tex_map_abuf[tex_id] = incppect.get_abuf('imgui.texture_data[%d]', tex_id);
            } else if (this.tex_map_abuf[tex_id] && (this.tex_map_id[tex_id] == null || this.tex_map_rev[tex_id] != tex_rev)) {
                this.tex_map_abuf[tex_id] = incppect.get_abuf('imgui.texture_data[%d]', tex_id);
                imgui_ws.init_tex(tex_id, tex_rev, this.tex_map_abuf[tex_id]);
            }
        }
    },

    init_tex: function(tex_id, tex_rev, tex_abuf) {
        var tex_abuf_uint8 = new Uint8Array(tex_abuf);
        var tex_abuf_int32 = new Int32Array(tex_abuf);

        const type = tex_abuf_int32[1];
        const width = tex_abuf_int32[2];
        const height = tex_abuf_int32[3];
        const revision = tex_abuf_int32[4];

        if (this.tex_map_rev[tex_id] && revision == this.tex_map_rev[tex_id]) {
            return;
        }

        var pixels = new Uint8Array(4*width*height);

        if (type == 0) { // Alpha8
            for (var i = 0; i < width*height; ++i) {
                pixels[4*i + 0] = 0xFF;
                pixels[4*i + 1] = 0xFF;
                pixels[4*i + 2] = 0xFF;
                pixels[4*i + 3] = tex_abuf_uint8[20 + i];
            }
        } else if (type == 1) { // Gray8
            for (var i = 0; i < width*height; ++i) {
                pixels[4*i + 0] = tex_abuf_uint8[20 + i];
                pixels[4*i + 1] = tex_abuf_uint8[20 + i];
                pixels[4*i + 2] = tex_abuf_uint8[20 + i];
                pixels[4*i + 3] = 0xFF;
            }
        } else if (type == 2) { // RGB24
            for (var i = 0; i < width*height; ++i) {
                pixels[4*i + 0] = tex_abuf_uint8[20 + 3*i + 0];
                pixels[4*i + 1] = tex_abuf_uint8[20 + 3*i + 1];
                pixels[4*i + 2] = tex_abuf_uint8[20 + 3*i + 2];
                pixels[4*i + 3] = 0xFF;
            }
        } else if (type == 3) { // RGBA32
            for (var i = 0; i < width*height; ++i) {
                pixels[4*i + 0] = tex_abuf_uint8[20 + 4*i + 0];
                pixels[4*i + 1] = tex_abuf_uint8[20 + 4*i + 1];
                pixels[4*i + 2] = tex_abuf_uint8[20 + 4*i + 2];
                pixels[4*i + 3] = tex_abuf_uint8[20 + 4*i + 3];
            }
        }

        this.tex_map_rev[tex_id] = tex_rev;

        if (this.tex_map_id[tex_id]) {
            this.gl.bindTexture(this.gl.TEXTURE_2D, this.tex_map_id[tex_id]);
            this.gl.texImage2D(this.gl.TEXTURE_2D, 0, this.gl.RGBA, width, height, 0, this.gl.RGBA, this.gl.UNSIGNED_BYTE, pixels);
        } else {
            this.tex_map_id[tex_id] = this.gl.createTexture();
            this.gl.bindTexture(this.gl.TEXTURE_2D, this.tex_map_id[tex_id]);
            this.gl.texParameteri(this.gl.TEXTURE_2D, this.gl.TEXTURE_MIN_FILTER, this.gl.LINEAR);
            this.gl.texParameteri(this.gl.TEXTURE_2D, this.gl.TEXTURE_MAG_FILTER, this.gl.LINEAR);
            //this.gl.pixelStorei(gl.UNPACK_ROW_LENGTH); // WebGL2
            this.gl.texImage2D(this.gl.TEXTURE_2D, 0, this.gl.RGBA, width, height, 0, this.gl.RGBA, this.gl.UNSIGNED_BYTE, pixels);
        }
    },

    incppect_draw_lists: function(incppect) {
        this.n_draw_lists = incppect.get_int32('imgui.n_draw_lists');
        if (this.n_draw_lists < 1) return;

        for (var i = 0; i < this.n_draw_lists; ++i) {
            this.draw_lists_abuf[i] = incppect.get_abuf('imgui.draw_list[%d]', i);
        }
    },

    render: function(n_draw_lists, draw_lists_abuf) {
        if (typeof n_draw_lists === "undefined" && typeof draw_lists_abuf === "undefined") {
            if (this.n_draw_lists === null) return;
            if (this.draw_lists_abuf === null) return;

            this.render(this.n_draw_lists, this.draw_lists_abuf);

            return;
        }

        var clip_off_x = 0.0;
        var clip_off_y = 0.0;

        this.gl.enable(this.gl.SCISSOR_TEST);

        for (var i_list = 0; i_list < n_draw_lists; ++i_list) {
            if (draw_lists_abuf[i_list].byteLength < 1) continue;

            var draw_data_offset = 0;

            var p = new Float32Array(draw_lists_abuf[i_list], draw_data_offset, 2);
            var offset_x = p[0]; draw_data_offset += 4;
            var offset_y = p[1]; draw_data_offset += 4;

            p = new Uint32Array(draw_lists_abuf[i_list], draw_data_offset, 1);
            var n_vertices = p[0]; draw_data_offset += 4;

            var av = new Float32Array(draw_lists_abuf[i_list], draw_data_offset, 5*n_vertices);

            for (var k = 0; k < n_vertices; ++k) {
                av[5*k + 0] += offset_x;
                av[5*k + 1] += offset_y;
            }

            this.gl.bindBuffer(this.gl.ARRAY_BUFFER, this.vertex_buffer);
            this.gl.bufferData(this.gl.ARRAY_BUFFER, av, this.gl.STREAM_DRAW);

            for (var k = 0; k < n_vertices; ++k) {
                av[5*k + 0] -= offset_x;
                av[5*k + 1] -= offset_y;
            }

            draw_data_offset += 5*4*n_vertices;

            p = new Uint32Array(draw_lists_abuf[i_list], draw_data_offset, 1);
            var n_indices = p[0]; draw_data_offset += 4;

            var ai = new Uint16Array(draw_lists_abuf[i_list], draw_data_offset, n_indices);
            this.gl.bindBuffer(this.gl.ELEMENT_ARRAY_BUFFER, this.index_buffer);
            this.gl.bufferData(this.gl.ELEMENT_ARRAY_BUFFER, ai, this.gl.STREAM_DRAW);
            draw_data_offset += 2*n_indices;

            p = new Uint32Array(draw_lists_abuf[i_list], draw_data_offset, 1);
            var n_cmd = p[0]; draw_data_offset += 4;

            for (var i_cmd = 0; i_cmd < n_cmd; ++i_cmd) {
                var pi = new Uint32Array(draw_lists_abuf[i_list], draw_data_offset, 4);
                var n_elements = pi[0]; draw_data_offset += 4;
                var texture_id = pi[1]; draw_data_offset += 4;
                var offset_vtx = pi[2]; draw_data_offset += 4;
                var offset_idx = pi[3]; draw_data_offset += 4;

                var pf = new Float32Array(draw_lists_abuf[i_list], draw_data_offset, 4);
                var clip_x = pf[0] - clip_off_x; draw_data_offset += 4;
                var clip_y = pf[1] - clip_off_y; draw_data_offset += 4;
                var clip_z = pf[2] - clip_off_x; draw_data_offset += 4;
                var clip_w = pf[3] - clip_off_y; draw_data_offset += 4;

                if (clip_x < this.canvas.width && clip_y < this.canvas.height && clip_z >= 0.0 && clip_w >= 0.0) {
                    this.gl.scissor(clip_x, this.canvas.height - clip_w, clip_z - clip_x, clip_w - clip_y);
                    if (texture_id in this.tex_map_id) {
                        this.gl.activeTexture(this.gl.TEXTURE0);
                        this.gl.bindTexture(this.gl.TEXTURE_2D, this.tex_map_id[texture_id]);
                    }
                    this.gl.drawElements(this.gl.TRIANGLES, n_elements, this.gl.UNSIGNED_SHORT, 2*offset_idx);
                }
            }
        }

        this.gl.disable(this.gl.SCISSOR_TEST);
    },

    canvas_on_keyup: function(event) {},
    canvas_on_keydown: function(event) {},
    canvas_on_keypress: function(event) {},
    canvas_on_pointermove: function(event) {},
    canvas_on_pointerdown: function(event) {},
    canvas_on_pointerup: function(event) {},
    canvas_on_wheel: function(event) {},
    canvas_on_touch: function(event) {},

    set_incppect_handlers: function(incppect) {
        this.canvas_on_keyup = function(event) {
            incppect.send('6 ' + event.keyCode);
        };

        this.canvas_on_keydown = function(event) {
            incppect.send('5 ' + event.keyCode);
        };

        this.canvas_on_keypress = function(event) {
            incppect.send('4 ' + event.keyCode);

            if (this.io.want_capture_keyboard) {
                event.preventDefault();
            }
        };

        this.canvas_on_pointermove = function(event) {
            this.io.mouse_x = event.offsetX * this.device_pixel_ratio;
            this.io.mouse_y = event.offsetY * this.device_pixel_ratio;

            incppect.send('0 ' + this.io.mouse_x + ' ' + this.io.mouse_y);

            if (this.io.want_capture_mouse) {
                event.preventDefault();
            }
        };

        this.canvas_on_pointerdown = function(event) {
            this.io.mouse_x = event.offsetX * this.device_pixel_ratio;
            this.io.mouse_y = event.offsetY * this.device_pixel_ratio;

            incppect.send('1 ' + event.button + ' ' + this.io.mouse_x + ' ' + this.io.mouse_y);
        };

        this.canvas_on_pointerup = function(event) {
            incppect.send('2 ' + event.button + ' ' + this.io.mouse_x + ' ' + this.io.mouse_y);

            if (this.io.want_capture_mouse) {
                event.preventDefault();
            }
        };

        this.canvas_on_wheel = function(event) {
            let scale = 1.0;
            switch (event.deltaMode) {
                case event.DOM_DELTA_PIXEL:
                    scale = 0.01;
                    break;
                case event.DOM_DELTA_LINE:
                    scale = 0.2;
                    break;
                case event.DOM_DELTA_PAGE:
                    scale = 1.0;
                    break;
            }

            var wheel_x =  event.deltaX * scale;
            var wheel_y = -event.deltaY * scale;

            incppect.send('3 ' + wheel_x + ' ' + wheel_y);

            if (this.io.want_capture_mouse) {
                event.preventDefault();
            }
        };

        this.canvas_on_touch = function (event) {
            if (event.touches.length > 1) {
                return;
            }

            var touches = event.changedTouches,
                first = touches[0],
                type = "";

            switch(event.type) {
                case "touchstart": type = "mousedown"; break;
                case "touchmove":  type = "mousemove"; break;
                case "touchend":   type = "mouseup";   break;
                default:           return;
            }

            // initMouseEvent(type, canBubble, cancelable, view, clickCount,
            //                screenX, screenY, clientX, clientY, ctrlKey,
            //                altKey, shiftKey, metaKey, button, relatedTarget);

            var simulatedEvent = document.createEvent("MouseEvent");
            simulatedEvent.initMouseEvent(type, true, true, window, 1, first.screenX, first.screenY, first.clientX, first.clientY, false, false, false, false, 0/*left*/, null);

            first.target.dispatchEvent(simulatedEvent);
            event.preventDefault();
        };
    },

}
