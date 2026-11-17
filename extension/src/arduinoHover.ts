/**
 * arduinoHover.ts
 * ───────────────
 * VS Code hover provider for the Arduino built-in API: shows concise
 * function docs from resources/arduino_docs.h. No clangd involved.
 */

import * as fs from "node:fs";
import * as vscode from "vscode";

export interface ArduinoSymbolDoc {
  brief: string;
  signature: string;
  params: Array<{ name: string; desc: string }>;
}

/**
 * Read `arduino_docs.h` and return a map of {symbolName → {brief, signature, params}}.
 * Returns an empty map on any I/O error so the rest of the extension keeps working.
 */
export function parseArduinoDocs(sidecarPath: string): Map<string, ArduinoSymbolDoc> {
  const docs = new Map<string, ArduinoSymbolDoc>();
  let content: string;
  try {
    content = fs.readFileSync(sidecarPath, "utf8");
  } catch {
    return docs;
  }

  const blockRe = /\/\*\*([\s\S]*?)\*\//g;
  let m: RegExpExecArray | null;
  while ((m = blockRe.exec(content)) !== null) {
    const body = m[1];

    // Pull out @brief ... up to (a) a blank comment line, (b) another @tag, or (c) end.
    const briefRe = /@brief\b\s*([\s\S]*?)(?=\n\s*\*\s*(?:@|\n)|\n\s*\*\/|$)/;
    const briefMatch = body.match(briefRe);
    if (!briefMatch) continue;

    const brief = briefMatch[1]
      .split("\n")
      .map((l) => l.replace(/^\s*\*\s?/, "").trim())
      .filter((l) => l.length > 0)
      .join(" ")
      .replace(/\s+/g, " ")
      .trim();
    if (!brief) continue;

    // Pull out @param name description entries.
    const params: Array<{ name: string; desc: string }> = [];
    const paramRe = /@param\s+(\w+)\s+([\s\S]*?)(?=\n\s*\*\s*(?:@|\n)|\n\s*\*\/|$)/g;
    let pm: RegExpExecArray | null;
    while ((pm = paramRe.exec(body)) !== null) {
      const desc = pm[2]
        .split("\n")
        .map((l) => l.replace(/^\s*\*\s?/, "").trim())
        .filter((l) => l.length > 0)
        .join(" ")
        .replace(/\s+/g, " ")
        .trim();
      if (pm[1] && desc) params.push({ name: pm[1], desc });
    }

    const after = content.slice(m.index + m[0].length, m.index + m[0].length + 400);
    const sym = identifySymbol(after);
    if (!sym) continue;
    if (!docs.has(sym.name)) docs.set(sym.name, { brief, signature: sym.signature, params });
  }
  return docs;
}

/**
 * Look at the text immediately after a doc-comment block and figure out which
 * symbol it documents. Returns {name, signature} or null if unrecognizable.
 */
function identifySymbol(after: string): { name: string; signature: string } | null {
  const firstLine = after.split("\n").map((l) => l.trim()).find((l) => l.length > 0) ?? "";

  const macro = after.match(/^\s*#define\s+([A-Za-z_]\w*)/);
  if (macro) {
    return { name: macro[1], signature: firstLine };
  }

  const ext = after.match(/^\s*extern\s+[\w:<>*&\s]+?\s+([A-Za-z_]\w*)\s*[;[]/);
  if (ext) {
    const sig = firstLine.replace(/^extern\s+/, "").replace(/;$/, "").trim();
    return { name: ext[1], signature: sig };
  }

  const cls = after.match(/^\s*class\s+([A-Za-z_]\w*)/);
  if (cls) {
    return { name: cls[1], signature: `class ${cls[1]}` };
  }

  const fn = after.match(
    /^\s*(?:(?:virtual|static|inline|explicit|constexpr|friend)\s+)*(?:[A-Za-z_][\w:]*\s*[*&\s]+)+([A-Za-z_]\w*)\s*\(/,
  );
  if (fn) {
    return { name: fn[1], signature: firstLine.replace(/;$/, "").trim() };
  }

  return null;
}

/**
 * Wire up the hover provider with a docsMap from parseArduinoDocs().
 */
export function registerArduinoHoverProvider(
  docsMap: Map<string, ArduinoSymbolDoc>,
  output: vscode.OutputChannel,
): vscode.Disposable {
  output.appendLine(`[hover-docs] Loaded ${docsMap.size} Arduino symbol descriptions.`);

  return vscode.languages.registerHoverProvider(
    [
      { scheme: "file", language: "cpp" },
      { scheme: "file", language: "c" },
      { scheme: "file", language: "arduino" },
    ],
    {
      provideHover(document, position) {
        const range = document.getWordRangeAtPosition(position);
        if (!range) return null;
        const word = document.getText(range);
        const doc = docsMap.get(word);
        if (!doc) return null;

        const md = new vscode.MarkdownString();
        md.appendMarkdown(`**function** \`${word}\`\n\n`);
        md.appendMarkdown(`${doc.brief}\n\n`);
        md.appendCodeblock(doc.signature, "cpp");
        for (const p of doc.params) {
          md.appendMarkdown(`*@param* \`${p.name}\` — ${p.desc}\n\n`);
        }
        md.isTrusted = false;
        return new vscode.Hover(md, range);
      },
    },
  );
}
