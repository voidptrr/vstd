# Benchmarks

`cstd.datastruct.benchmarks` reports comparative latency and throughput for container operation cases.

## Run

```bash
nix run .#bench-all
```

## Environment

- OS/arch: `Linux 6.18.24` (`x86_64`)
- CPU: `AMD Ryzen AI 9 HX PRO 370 w/ Radeon 890M`
- Topology: `12 cores / 24 threads`
- CPU governor during run: `powersave`
- Platform profile during run: `balanced`

## Vector

<div class="bench-grid">
  <div class="bench-card">
    <h3>Latency</h3>
    <table>
      <thead><tr><th>Case</th><th>Median (ns/op)</th></tr></thead>
      <tbody>
        <tr><td>Steady push + pop</td><td>5.054</td></tr>
        <tr><td>Growth push + pop</td><td>8.319</td></tr>
      </tbody>
    </table>
  </div>
  <div class="bench-card">
    <h3>Throughput</h3>
    <table>
      <thead><tr><th>Case</th><th>Throughput (ops/sec)</th></tr></thead>
      <tbody>
        <tr><td>Steady push + pop</td><td>197,860,420</td></tr>
        <tr><td>Growth push + pop</td><td>120,208,002</td></tr>
      </tbody>
    </table>
  </div>
</div>

## Queue

<div class="bench-grid">
  <div class="bench-card">
    <h3>Latency</h3>
    <table>
      <thead><tr><th>Case</th><th>Median (ns/op)</th></tr></thead>
      <tbody>
        <tr><td>Steady push + pop</td><td>4.990</td></tr>
        <tr><td>Growth push + pop</td><td>12.240</td></tr>
        <tr><td>Steady push front + pop back</td><td>4.947</td></tr>
        <tr><td>Growth push front + pop back</td><td>12.141</td></tr>
      </tbody>
    </table>
  </div>
  <div class="bench-card">
    <h3>Throughput</h3>
    <table>
      <thead><tr><th>Case</th><th>Throughput (ops/sec)</th></tr></thead>
      <tbody>
        <tr><td>Steady push + pop</td><td>200,400,702</td></tr>
        <tr><td>Growth push + pop</td><td>81,698,228</td></tr>
        <tr><td>Steady push front + pop back</td><td>202,128,643</td></tr>
        <tr><td>Growth push front + pop back</td><td>82,364,056</td></tr>
      </tbody>
    </table>
  </div>
</div>

## Linked list

<div class="bench-grid">
  <div class="bench-card">
    <h3>Latency</h3>
    <table>
      <thead><tr><th>Case</th><th>Median (ns/op)</th></tr></thead>
      <tbody>
        <tr><td>Steady push</td><td>18.820</td></tr>
        <tr><td>Push then free</td><td>24.903</td></tr>
        <tr><td>Steady push front + pop left</td><td>13.589</td></tr>
      </tbody>
    </table>
  </div>
  <div class="bench-card">
    <h3>Throughput</h3>
    <table>
      <thead><tr><th>Case</th><th>Throughput (ops/sec)</th></tr></thead>
      <tbody>
        <tr><td>Steady push</td><td>53,134,343</td></tr>
        <tr><td>Push then free</td><td>40,156,382</td></tr>
        <tr><td>Steady push front + pop left</td><td>73,589,620</td></tr>
      </tbody>
    </table>
  </div>
</div>

## Notes

- Steady: fixed-size steady-state loop.
- Growth: includes expansion/allocation behavior.
