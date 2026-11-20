# INFO1 first screen

Supplied coolant temperature / oil pressure / transmission temperature layout.
Only the INFO1 heading is hidden, leaving blank space between its red accents
for a future header. Face fills the 720 x 720 display; values are native LVGL
widgets and update only when their rounded display values change.

GAUGE_DEMO_MODE=1 uses slow display-only sample values around 195 F, 58 PSI,
172 F. With demo disabled, coolant and oil pressure use existing data sources.
Transmission remains -- until a verified source is connected through
ui_info1_set_values(coolant_f, oil_psi, transmission_f) from LVGL context.
NAN displays --. No transmission CAN protocol is invented by this UI change.

Validated Base display initialization, DMA buffers and DSI compatibility are
preserved. Hardware build, visual alignment and data-source checks remain needed.
