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
        <tr><td>Steady push + pop</td><td>29.748</td></tr>
        <tr><td>Growth push + pop</td><td>79.646</td></tr>
        <tr><td>Steady peek</td><td>2.308</td></tr>
      </tbody>
    </table>
  </div>
  <div class="bench-card">
    <h3>Throughput</h3>
    <table>
      <thead><tr><th>Case</th><th>Throughput (ops/sec)</th></tr></thead>
      <tbody>
        <tr><td>Steady push + pop</td><td>33,615,154</td></tr>
        <tr><td>Growth push + pop</td><td>12,555,483</td></tr>
        <tr><td>Steady peek</td><td>433,216,287</td></tr>
      </tbody>
    </table>
  </div>
</div>

## Hashmap

<div class="bench-grid">
  <div class="bench-card">
    <h3>Latency</h3>
    <table>
      <thead><tr><th>Case</th><th>Median (ns/op)</th></tr></thead>
      <tbody>
        <tr><td>Steady put + get</td><td>15.925</td></tr>
        <tr><td>Growth put + get</td><td>546.275</td></tr>
        <tr><td>Steady get hit</td><td>6.958</td></tr>
      </tbody>
    </table>
  </div>
  <div class="bench-card">
    <h3>Throughput</h3>
    <table>
      <thead><tr><th>Case</th><th>Throughput (ops/sec)</th></tr></thead>
      <tbody>
        <tr><td>Steady put + get</td><td>62,796,011</td></tr>
        <tr><td>Growth put + get</td><td>1,830,579</td></tr>
        <tr><td>Steady get hit</td><td>143,723,595</td></tr>
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
        <tr><td>Steady push + pop</td><td>5.184</td></tr>
        <tr><td>Growth push + pop</td><td>8.514</td></tr>
      </tbody>
    </table>
  </div>
  <div class="bench-card">
    <h3>Throughput</h3>
    <table>
      <thead><tr><th>Case</th><th>Throughput (ops/sec)</th></tr></thead>
      <tbody>
        <tr><td>Steady push + pop</td><td>192,894,354</td></tr>
        <tr><td>Growth push + pop</td><td>117,457,787</td></tr>
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
        <tr><td>Steady push + pop</td><td>5.189</td></tr>
        <tr><td>Growth push + pop</td><td>12.565</td></tr>
        <tr><td>Steady push front + pop back</td><td>4.996</td></tr>
        <tr><td>Growth push front + pop back</td><td>12.504</td></tr>
      </tbody>
    </table>
  </div>
  <div class="bench-card">
    <h3>Throughput</h3>
    <table>
      <thead><tr><th>Case</th><th>Throughput (ops/sec)</th></tr></thead>
      <tbody>
        <tr><td>Steady push + pop</td><td>192,707,038</td></tr>
        <tr><td>Growth push + pop</td><td>79,588,706</td></tr>
        <tr><td>Steady push front + pop back</td><td>200,151,740</td></tr>
        <tr><td>Growth push front + pop back</td><td>79,972,784</td></tr>
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
        <tr><td>Steady push</td><td>17.218</td></tr>
        <tr><td>Push then free</td><td>30.895</td></tr>
        <tr><td>Steady push front + pop left</td><td>14.768</td></tr>
      </tbody>
    </table>
  </div>
  <div class="bench-card">
    <h3>Throughput</h3>
    <table>
      <thead><tr><th>Case</th><th>Throughput (ops/sec)</th></tr></thead>
      <tbody>
        <tr><td>Steady push</td><td>58,080,266</td></tr>
        <tr><td>Push then free</td><td>32,367,474</td></tr>
        <tr><td>Steady push front + pop left</td><td>67,714,563</td></tr>
      </tbody>
    </table>
  </div>
</div>

## Notes

- Steady: fixed-size steady-state loop.
- Growth: includes expansion/allocation behavior.
