package mmx.x6.game.enemy.heatnix;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

import mmx.x6.stage.StageRun;
import mmx.x6.stage.Stages;
import org.junit.jupiter.api.Test;

class GreyBlockTest {

    @Test
    void fallingBlockSqueezesXButDoesNotBuryHim() {
        StageRun r = StageRun.at(Stages.HEATNIX, 1);
        r.steps(150, 0);
        GreyBlock block = new GreyBlock(r.world, new mmx.x6.level.RecordDef(-1, "GreyBlock", "stage", 4, 12, 9,
                0, (int) r.world.player.x, (int) r.world.player.y - 80));
        r.world.entities.add(block);
        float floor = r.world.player.hurtBox().bottom();
        int hp = r.world.player.hp;
        for (int i = 0; i < 200; i++) {
            r.step(0);
            assertTrue(r.world.player.hurtBox() == null || r.world.player.hurtBox().bottom() <= floor + 0.01f);
        }
        assertTrue(r.world.player.hp < hp);
        assertEquals(GreyBlock.Step.REST, block.step());
    }
}
