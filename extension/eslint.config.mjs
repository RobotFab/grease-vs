// ESLint flat config (ESLint 9+). Uses only the packages already in devDependencies.
import tsParser from "@typescript-eslint/parser";
import tsPlugin from "@typescript-eslint/eslint-plugin";

export default [
  { ignores: ["dist/**", "out/**", "node_modules/**", "tmp/**", "resources/**", "snippets/**"] },
  ...tsPlugin.configs["flat/recommended"],
  {
    files: ["**/*.ts"],
    languageOptions: { parser: tsParser, ecmaVersion: 2022, sourceType: "module" },
  },
  {
    linterOptions: { reportUnusedDisableDirectives: "off" },
    rules: {
      // any is used on purpose for JSON from arduino-cli, serial-discovery and webviews.
      "@typescript-eslint/no-explicit-any": "off",
      "@typescript-eslint/no-unused-vars": ["error", { argsIgnorePattern: "^_", caughtErrorsIgnorePattern: "^_" }],
    },
  },
];
