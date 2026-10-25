package mmx.x6.render;

import com.badlogic.gdx.graphics.glutils.ShaderProgram;

public final class FlashShader {

    private static final String VERTEX = """
            attribute vec4 a_position;
            attribute vec4 a_color;
            attribute vec2 a_texCoord0;
            uniform mat4 u_projTrans;
            varying vec4 v_color;
            varying vec2 v_texCoords;
            void main() {
                v_color = a_color;
                v_color.a = v_color.a * (255.0 / 254.0);
                v_texCoords = a_texCoord0;
                gl_Position = u_projTrans * a_position;
            }
            """;

    private static final String FRAGMENT = """
            #ifdef GL_ES
            precision mediump float;
            #endif
            varying vec4 v_color;
            varying vec2 v_texCoords;
            uniform sampler2D u_texture;
            void main() {
                vec4 c = v_color * texture2D(u_texture, v_texCoords);
                float lum = dot(c.rgb, vec3(0.299, 0.587, 0.114));
                vec3 ramp = mix(vec3(140.0, 222.0, 255.0) / 255.0, vec3(1.0), lum);
                gl_FragColor = vec4(ramp, c.a);
            }
            """;

    private FlashShader() {
    }

    public static ShaderProgram create() {
        ShaderProgram p = new ShaderProgram(VERTEX, FRAGMENT);
        if (!p.isCompiled()) {
            throw new IllegalStateException("flash shader: " + p.getLog());
        }
        return p;
    }
}
