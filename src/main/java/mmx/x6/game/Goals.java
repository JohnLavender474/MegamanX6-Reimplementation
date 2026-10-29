package mmx.x6.game;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;
import mmx.x6.level.GoalDef;
import mmx.x6.level.GoalMode;
import mmx.x6.level.RecordDef;

public final class Goals {

    private final Map<Integer, GoalDef> goals = new HashMap<>();
    private final Map<Integer, List<RecordDef>> recordMembers = new HashMap<>();
    private final Map<Integer, List<Integer>> goalMembers = new HashMap<>();
    private final Set<RecordDef> killed = new HashSet<>();
    private final Set<Integer> done = new HashSet<>();

    public Goals(List<GoalDef> goalDefs, List<RecordDef> records, List<RecordDef> fixedRecords) {
        for (GoalDef g : goalDefs) {
            goals.put(g.tmxId(), g);
        }
        for (GoalDef g : goalDefs) {
            if (g.parent() != 0 && goals.containsKey(g.parent())) {
                goalMembers.computeIfAbsent(g.parent(), k -> new ArrayList<>()).add(g.tmxId());
            }
        }
        for (List<RecordDef> list : List.of(records, fixedRecords)) {
            for (RecordDef r : list) {
                if (r.goal() != 0 && goals.containsKey(r.goal())) {
                    recordMembers.computeIfAbsent(r.goal(), k -> new ArrayList<>()).add(r);
                }
            }
        }
    }

    public boolean isEmpty() {
        return goals.isEmpty();
    }

    public boolean done(int goalTmxId) {
        return done.contains(goalTmxId);
    }

    public void reset() {
        killed.clear();
        done.clear();
    }

    public void reportKill(World w, RecordDef r) {
        if (r == null || !goals.containsKey(r.goal()) || !recordMembers.getOrDefault(r.goal(), List.of()).contains(r)) {
            return;
        }
        killed.add(r);
        evaluate(w, r.goal(), new HashSet<>());
    }

    public void reportBossEnd(World w, RecordDef r) {
        if (isEmpty()) {
            w.flow.clear();
        } else {
            reportKill(w, r);
        }
    }

    private void evaluate(World w, int id, Set<Integer> visited) {
        if (!visited.add(id)) {
            return;
        }
        GoalDef g = goals.get(id);
        if (g == null || done.contains(id)) {
            return;
        }
        int total = 0;
        int count = 0;
        for (RecordDef r : recordMembers.getOrDefault(id, List.of())) {
            total++;
            if (killed.contains(r)) {
                count++;
            }
        }
        for (int child : goalMembers.getOrDefault(id, List.of())) {
            total++;
            if (done.contains(child)) {
                count++;
            }
        }
        boolean complete = total > 0 && (g.mode() == GoalMode.ALL ? count == total : count > 0);
        if (!complete) {
            return;
        }
        done.add(id);
        if (g.endsStage()) {
            w.flow.clear();
        }
        if (g.parent() != 0) {
            evaluate(w, g.parent(), visited);
        }
    }
}
