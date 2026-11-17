/**
 * Boards with native USB drop their port for a moment on every upload and
 * reset. A port counts as unplugged only when it stays missing this long,
 * counted from the end of the last build or upload. Otherwise the target an
 * agent chose would be cleared by its own upload.
 */
export const PORT_GONE_MS = 4000;

export class PortGoneTimer {
  private missingSince: number | null = null;

  /** Call on every board scan. True when the target's port is really gone. */
  isGone(portPresent: boolean, lastBusyAtMs: number, nowMs: number = Date.now()): boolean {
    if (portPresent) {
      this.missingSince = null;
      return false;
    }
    if (this.missingSince === null) this.missingSince = nowMs;
    return nowMs - Math.max(this.missingSince, lastBusyAtMs) >= PORT_GONE_MS;
  }

  reset(): void {
    this.missingSince = null;
  }
}
