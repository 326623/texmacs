<TeXmacs|1.99.11>

<style|source>

<\body>
  <active*|<\src-title>
    <src-package|shadowed-titles|1.0>

    <\src-purpose>
      Attach shadows to title bars
    </src-purpose>

    <src-copyright|2019|Joris van der Hoeven>

    <\src-license>
      This software falls under the <hlink|GNU general public license,
      version 3 or later|$TEXMACS_PATH/LICENSE>. It comes WITHOUT ANY
      WARRANTY WHATSOEVER. You should have received a copy of the license
      which the software. If not, see <hlink|http://www.gnu.org/licenses/gpl-3.0.html|http://www.gnu.org/licenses/gpl-3.0.html>.
    </src-license>
  </src-title>>

  <assign|title-sub-bar-frame|<macro|body|<deco-title-shadow-bar|<arg|body>>>>
</body>

<\initial>
  <\collection>
    <associate|sfactor|7>
  </collection>
</initial>