package mmx.x6.stage;

import com.badlogic.gdx.utils.XmlReader;
import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.ArrayList;
import java.util.List;
import java.util.stream.Collectors;
import mmx.x6.DataPaths;
import mmx.x6.level.Catalog;
import mmx.x6.level.Finding;
import mmx.x6.level.Level;
import mmx.x6.level.LevelValidator;
import mmx.x6.level.TmxLoader;
import mmx.x6.level.ValidationReport;

public final class StageLoader {

    private StageLoader() {
    }

    public static LoadedStage builtIn(DataPaths p, StageDef def) throws IOException {
        Path file = p.tmx(def.tmxFile());
        String text = Files.readString(file, StandardCharsets.UTF_8);
        if (!hasProfile(text, def.tmxFile())) {
            // TODO: the six other stages bypass the validator and keep their data in Stages until each moves to the profile (docs/superpowers/specs/2026-10-29-user-levels-design.md, "Validator")
            return new LoadedStage(def, TmxLoader.load(file), null, null);
        }
        Catalog catalog = Catalog.load(p.catalogJson());
        ValidationReport report = validate(p, catalog, text);
        if (report.status() == ValidationReport.Status.REJECTED) {
            throw new IOException(describe(def.tmxFile(), report));
        }
        Level level = TmxLoader.fromElement(report.map(), def.tmxFile());
        return new LoadedStage(def.withMapData(level), level, report, tilesOf(catalog, level));
    }

    public static LoadedStage file(DataPaths p, Path tmx) throws IOException {
        String name = tmx.getFileName() == null ? tmx.toString() : tmx.getFileName().toString();
        String text;
        try {
            text = Files.readString(tmx, StandardCharsets.UTF_8);
        } catch (IOException | RuntimeException e) {
            return rejected(List.of(), "read", "The file " + tmx + " cannot be read: " + e);
        }
        Catalog catalog = Catalog.load(p.catalogJson());
        ValidationReport report = validate(p, catalog, text);
        if (report.status() == ValidationReport.Status.REJECTED) {
            return new LoadedStage(null, null, report, null);
        }
        try {
            Level level = TmxLoader.fromElement(report.map(), name);
            StageDef def = new StageDef("level", name, 0, name, List.of(), 0, 0, null, null, null, 0,
                    Themes.factory(level.properties().get("x6_theme"))).withMapData(level);
            return new LoadedStage(def, level, report, tilesOf(catalog, level));
        } catch (IOException | RuntimeException e) {
            return rejected(report.findings(), "load", "The level passed the validator, but the loader failed: " + e);
        }
    }

    public static LoadedStage unreadable(String file, RuntimeException e) {
        return rejected(List.of(), "read", "The file " + file + " cannot be read: " + e);
    }

    private static LoadedStage rejected(List<Finding> before, String code, String message) {
        List<Finding> findings = new ArrayList<>(before);
        findings.add(new Finding(Finding.Result.REJECT, code, message));
        return new LoadedStage(null, null, new ValidationReport(List.copyOf(findings), null), null);
    }

    private static String tilesOf(Catalog catalog, Level level) {
        String theme = level.properties().get("x6_theme");
        return theme == null ? null : Themes.tilesPng(catalog, theme);
    }

    private static boolean hasProfile(String text, String name) throws IOException {
        try {
            XmlReader.Element map = new XmlReader().parse(text);
            XmlReader.Element props = map.getChildByName("properties");
            if (props == null) {
                return false;
            }
            for (XmlReader.Element e : props.getChildrenByName("property")) {
                if ("x6_profile".equals(e.getAttribute("name", ""))) {
                    return true;
                }
            }
            return false;
        } catch (RuntimeException e) {
            throw new IOException("cannot parse " + name + ": " + e.getMessage(), e);
        }
    }

    private static ValidationReport validate(DataPaths p, Catalog catalog, String text) {
        return LevelValidator.validate(text, catalog, name -> {
            try {
                return Files.readString(p.tmx(name), StandardCharsets.UTF_8);
            } catch (IOException e) {
                return null;
            }
        });
    }

    private static String describe(String name, ValidationReport report) {
        return name + " is rejected: " + report.findings().stream()
                .filter(f -> f.result() == Finding.Result.REJECT)
                .map(f -> f.code() + ": " + f.message())
                .collect(Collectors.joining("; "));
    }
}
