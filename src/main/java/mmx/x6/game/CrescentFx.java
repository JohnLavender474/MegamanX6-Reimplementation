package mmx.x6.game;

import java.util.ArrayList;
import java.util.List;

public record CrescentFx(float cx, float cy, float radius, float thickness, float fromDeg, float toDeg,
                         boolean faceRight, float r, float g, float b, float a) {

    public List<float[]> outline(int segments) {
        List<float[]> out = new ArrayList<>(segments + 1);
        float face = faceRight ? 1 : -1;
        for (int i = 0; i <= segments; i++) {
            float t = i / (float) segments;
            double angle = Math.toRadians(fromDeg + (toDeg - fromDeg) * t);
            float th = thickness * (float) Math.sin(Math.PI * t);
            float cos = (float) Math.cos(angle);
            float sin = (float) Math.sin(angle);
            float inner = radius - th;
            out.add(new float[] {cx + face * cos * radius, cy + sin * radius, cx + face * cos * inner, cy + sin * inner});
        }
        return out;
    }
}
