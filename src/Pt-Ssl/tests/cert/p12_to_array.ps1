param(
    [string]$InputFile,
    [string]$VarName
)
$bytes = [System.IO.File]::ReadAllBytes($InputFile)
$sb = New-Object System.Text.StringBuilder
$null = $sb.AppendLine("const unsigned char ${VarName}[] = {")
for($i = 0; $i -lt $bytes.Length; $i += 16) {
    $end = [Math]::Min($i + 15, $bytes.Length - 1)
    $chunk = $bytes[$i..$end]
    $hex = ($chunk | ForEach-Object { "$_" }) -join ", "
    if($i -gt 0) { $null = $sb.Append(",`n") }
    $null = $sb.Append("    $hex")
}
$null = $sb.Append("`n};")
Write-Output $sb.ToString()
