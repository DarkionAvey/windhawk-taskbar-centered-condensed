
std::vector<std::wstring> SplitAndTrim(
    const std::optional<std::wstring>& input) {
  std::vector<std::wstring> result;

  if (!input.has_value() || input->empty()) {
    return result;
  }
  std::wstringstream ss(*input);
  std::wstring item;
  while (std::getline(ss, item, L';')) {
    size_t start = item.find_first_not_of(L" \t");
    size_t end = item.find_last_not_of(L" \t");

    if (start != std::wstring::npos && end != std::wstring::npos) {
      std::wstring trimmed = item.substr(start, end - start + 1);
      if (!trimmed.empty()) {
        result.push_back(trimmed);
      }
    }
  }
  return result;
}

bool RegexMatchInsensitive(const std::wstring& haystack,
                           const std::wstring& pattern) {
  try {
    std::wregex regexPattern(pattern, std::regex_constants::icase);
    return std::regex_search(haystack, regexPattern);
  } catch (const std::regex_error&) {
    return false;
  }
}

bool ContainsCaseInsensitive(std::wstring const& haystack,
                             std::wstring const& needle) {
  std::wstring h = haystack, n = needle;
  std::transform(h.begin(), h.end(), h.begin(), towlower);
  std::transform(n.begin(), n.end(), n.begin(), towlower);
  return h.find(n) != std::wstring::npos;
}
