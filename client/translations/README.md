# create .pot
find . -type f \( -name "*.cpp" -o -name "*.h" \) | xgettext -k_ --c++ -f - -o translations/client.pot

# create .po
msginit -i translations/client.pot -l ru -o translations/ru/LC_MESSAGES/client.po

# create .mo
msgfmt -o translations/ru/LC_MESSAGES/client.mo translations/ru/LC_MESSAGES/client.po
