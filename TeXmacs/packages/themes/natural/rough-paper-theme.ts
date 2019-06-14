<TeXmacs|1.99.9>

<style|source>

<\body>
  <active*|<\src-title>
    <src-package-dtd|rough-paper-theme|1.0|rough-paper-theme|1.0>

    <\src-purpose>
      Ridged paper theme for presentations and posters.
    </src-purpose>

    <src-copyright|2013--2019|Joris van der Hoeven>

    <\src-license>
      This software falls under the <hlink|GNU general public license,
      version 3 or later|$TEXMACS_PATH/LICENSE>. It comes WITHOUT ANY
      WARRANTY WHATSOEVER. You should have received a copy of the license
      which the software. If not, see <hlink|http://www.gnu.org/licenses/gpl-3.0.html|http://www.gnu.org/licenses/gpl-3.0.html>.
    </src-license>
  </src-title>>

  <use-package|reddish-theme|paper-rough-ornament>

  <copy-theme|rough-paper|reddish>

  <select-theme|rough-paper|paper-rough>

  <\active*>
    <\src-comment>
      Background
    </src-comment>
  </active*>

  <assign|rough-paper-bg-color|<pattern|paper-rough-bright.png|*3/5|*3/5|#f8f8f8>>

  <assign|rough-paper-monochrome-bg-color|#f8f8f8>

  <\active*>
    <\src-comment>
      Standard ornaments
    </src-comment>
  </active*>

  <assign|rough-paper-ornament-color|<pattern|paper-rough-medium.png|*3/5|*3/5|#dcdcdc>>

  <assign|rough-paper-ornament-extra-color|<pattern|wood-medium.png|*3/5|*3/5|#804018>>

  <assign|rough-paper-ornament-sunny-color|#e0e0e0>

  <assign|rough-paper-ornament-shadow-color|#a0a0a0>

  <\active*>
    <\src-comment>
      Titles
    </src-comment>
  </active*>

  <assign|rough-paper-title-bar-color|<pattern|wood-medium.png|*3/5|*3/5|#804018>>

  <\active*>
    <\src-comment>
      Sessions
    </src-comment>
  </active*>

  <assign|rough-paper-input-color|<pattern|paper-rough-light.png|*3/5|*3/5|#eeeeee>>

  <assign|rough-paper-fold-bar-color|<pattern|paper-rough-medium.png|*3/5|*3/5|#dcdcdc>>

  <assign|rough-paper-fold-title-color|<pattern|paper-rough-medium.png|*3/5|*3/5|#dcdcdc>>

  <\active*>
    <\src-comment>
      Posters
    </src-comment>
  </active*>

  <copy-ornament|rough-paper-title|paper-rough>

  <copy-ornament|rough-paper-framed|paper-rough>
</body>

<\initial>
  <\collection>
    <associate|sfactor|7>
  </collection>
</initial>