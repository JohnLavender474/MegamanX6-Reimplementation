package mmx.x6.anim;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assumptions.assumeTrue;

import java.nio.file.Files;
import mmx.x6.DataPaths;
import org.junit.jupiter.api.Test;

class AnimSetTest {

    @Test
    void parsesTheJunkroidSet() throws Exception {
        assumeTrue(DataPaths.assetsExist());
        DataPaths p = DataPaths.assets();
        AnimSet s = AnimSet.parse(Files.readString(p.enemyDir("junkroid").resolve("animations.json")));
        assertEquals(66, s.cellW());
        assertEquals(61, s.cellH());
        assertEquals(32, s.originX());
        assertEquals(29, s.originY());
        assertEquals(8, s.indices().size());
        AnimDef shoot = s.get(1);
        assertEquals(6, shoot.frames().size());
        assertEquals(1, shoot.frames().get(2).flag());
        assertEquals(2, shoot.frames().get(5).flag());
    }
}
