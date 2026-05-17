# Benchmarks

`cstd.datastruct.benchmarks` reports comparative latency and throughput for container operation cases.

## Run

```bash
nix develop -c bench-all
```

## Environment

- OS/arch: `Linux 6.18.24` (`x86_64`)
- CPU: `AMD Ryzen AI 9 HX PRO 370 w/ Radeon 890M`
- Topology: `12 cores / 24 threads`
- CPU governor during run: `powersave`
- Platform profile during run: `balanced`

## Binary heap

<div class="bench-grid">
  <div class="bench-card">
    <h3>Latency</h3>
    <table>
      <thead><tr><th>Case</th><th>Median (ns/op)</th></tr></thead>
      <tbody>
        <tr><td>Steady push + pop</td><td>27.782</td></tr>
        <tr><td>Growth push + pop</td><td>72.765</td></tr>
        <tr><td>Steady peek</td><td>2.257</td></tr>
      </tbody>
    </table>
  </div>
  <div class="bench-card">
    <h3>Throughput</h3>
    <table>
      <thead><tr><th>Case</th><th>Throughput (ops/sec)</th></tr></thead>
      <tbody>
        <tr><td>Steady push + pop</td><td>35,994,794</td></tr>
        <tr><td>Growth push + pop</td><td>13,742,925</td></tr>
        <tr><td>Steady peek</td><td>443,077,128</td></tr>
      </tbody>
    </table>
  </div>
</div>

## Vector

<div class="bench-grid">
  <div class="bench-card">
    <h3>Latency</h3>
    <table>
      <thead><tr><th>Case</th><th>Median (ns/op)</th></tr></thead>
      <tbody>
        <tr><td>Steady push + pop</td><td>5.288</td></tr>
        <tr><td>Growth push + pop</td><td>8.619</td></tr>
      </tbody>
    </table>
  </div>
  <div class="bench-card">
    <h3>Throughput</h3>
    <table>
      <thead><tr><th>Case</th><th>Throughput (ops/sec)</th></tr></thead>
      <tbody>
        <tr><td>Steady push + pop</td><td>189,104,910</td></tr>
        <tr><td>Growth push + pop</td><td>116,023,582</td></tr>
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
        <tr><td>Steady push + pop</td><td>5.071</td></tr>
        <tr><td>Growth push + pop</td><td>12.645</td></tr>
        <tr><td>Steady push front + pop back</td><td>4.943</td></tr>
        <tr><td>Growth push front + pop back</td><td>12.077</td></tr>
      </tbody>
    </table>
  </div>
  <div class="bench-card">
    <h3>Throughput</h3>
    <table>
      <thead><tr><th>Case</th><th>Throughput (ops/sec)</th></tr></thead>
      <tbody>
        <tr><td>Steady push + pop</td><td>197,191,693</td></tr>
        <tr><td>Growth push + pop</td><td>79,081,021</td></tr>
        <tr><td>Steady push front + pop back</td><td>202,310,293</td></tr>
        <tr><td>Growth push front + pop back</td><td>82,800,250</td></tr>
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
        <tr><td>Steady push</td><td>16.822</td></tr>
        <tr><td>Push then free</td><td>28.569</td></tr>
        <tr><td>Steady push front + pop left</td><td>13.305</td></tr>
      </tbody>
    </table>
  </div>
  <div class="bench-card">
    <h3>Throughput</h3>
    <table>
      <thead><tr><th>Case</th><th>Throughput (ops/sec)</th></tr></thead>
      <tbody>
        <tr><td>Steady push</td><td>59,446,167</td></tr>
        <tr><td>Push then free</td><td>35,002,921</td></tr>
        <tr><td>Steady push front + pop left</td><td>75,158,960</td></tr>
      </tbody>
    </table>
  </div>
</div>

## Notes

- Steady: fixed-size steady-state loop.
- Growth: includes expansion/allocation behavior.
